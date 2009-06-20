#include "private_evlua.h"
#include <lua.h>
#include <lauxlib.h>
#include <string.h>

/**
 * Private information.
 */
static int evlua_live_objects = 0;
static int evlua_registry = 0;

//////////////////////////////////////////////////////////////////////
void evlua_cb(struct ev_loop *loop, int watcher_ref, int revents, int is_active) {
    // PRECONDITION: Loop object should be last element on the stack!
    lua_State* L = evlua_loop_state();
    evlua_reg_get(L, watcher_ref); // <loop>, <watcher>
    if ( lua_isnil(L, -1) ) {
        // TODO: Handle OOM...
        lua_pushfstring(L,
                        "InternalError: event callback executed but is missing the 'self' field at %s line %d\n",
                        __FILE__, __LINE__);
        goto ERROR; // <loop>, <watcher>, ERR
    }
    if ( ! is_active ) evlua_loop_unref(L, -2, -1);
    lua_getfenv(L, -1); // <loop>, <watcher>, {func}
    if ( lua_isnil(L, -1) ) {
        lua_pop(L, 1); // <loop>, <watcher>
        // TODO: Handle OOM...
        lua_pushfstring(L,
                        "MissingUserDataEnv: User data is missing the environment at %s line %d\n",
                        __FILE__, __LINE__);
        goto ERROR; // <loop>, <watcher>, ERR
    }
    lua_rawgeti(L, -1, EVLUA_WATCHER_FUNCTION);
    // <loop>, <watcher>, {func}, func
    if ( lua_isnil(L, -1) ) {
        lua_pop(L, 2); // <loop>, <watcher>
        // TODO: Handle OOM...
        lua_pushfstring(L,
                        "MissingCallback: Internal field of an event was modified so the callback function is missing at %s line %d\n",
                        __FILE__, __LINE__);
        // <loop>, <watcher>, ERR
        goto ERROR;
    }
    lua_remove(L, -2);           // <loop>, <watcher>, func
    lua_pushvalue(L, -3);        // <loop>, <watcher>, func, <loop>
    lua_pushvalue(L, -3);        // <loop>, <watcher>, func, <loop>, <watcher>
    lua_pushinteger(L, revents); // <loop>, <watcher>, func, <loop>, <watcher>, revents

    // The show must go on, so ignore any error.
    int err = lua_pcall(L, 3, 0, 0);
    // <loop>, <watcher> [, ERR]
    if ( err ) {
      ERROR: // <loop>, <watcher>, ERR
        // TODO: add low level debug statement.
        fprintf(stderr, "FAILED CALLBACK: %s\n",
                lua_tostring(L, -1));

        lua_pop(L, 1); // <loop>, <watcher>
    }
    lua_pop(L, 1); // <loop>
}

//////////////////////////////////////////////////////////////////////
void evlua_reg_delete(lua_State* L) {
    --evlua_live_objects;
}

//////////////////////////////////////////////////////////////////////
int evlua_reg_count(lua_State *L) {
    lua_pushinteger(L, evlua_live_objects);
    return 1;
}

//////////////////////////////////////////////////////////////////////
void evlua_reg_get(lua_State* L, int ref) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, evlua_registry); // {reg}
    lua_rawgeti(L, -1, ref); // {reg}, {obj}
    lua_remove(L, -2); // {obj}
}

//////////////////////////////////////////////////////////////////////
void* evlua_watcher_new(lua_State *L, size_t c_type_size, char* lua_type, int* ref) {
    // Create the object:
    void *obj = lua_newuserdata(L, c_type_size); // ud

    // Set the metatable for the userdata:
    luaL_getmetatable(L, lua_type); // ud, {meta}
    lua_setmetatable(L, -2); // ud

    // Set the fenv for the userdata:
    lua_createtable(L, 0, 1);                   // ud, {}
    lua_pushvalue(L, 1);                        // ud, {}, func
    lua_rawseti(L, -2, EVLUA_WATCHER_FUNCTION); // ud, {}
    lua_setfenv(L, -2);                         // ud

    // Register the userdata:
    lua_rawgeti(L, LUA_REGISTRYINDEX, evlua_registry); // ud, {reg}
    lua_pushvalue(L, -2); // {}, {reg}, {}
    *ref = luaL_ref(L, -2); // {}, {reg}
    lua_pop(L, 1); // {}
    evlua_live_objects++;

    return obj;
}

//////////////////////////////////////////////////////////////////////
void evlua_open_shared(lua_State *L) {
    // Create the global weak refs table which is simply a set of
    // evlua objects that exist.  The key into this table is an
    // integer (assigned via luaL_ref), and the value is a lua table
    // appropriate for the type of object.
    lua_newtable(L); // {}
    lua_createtable(L, 0, 1); // {}, {}
    lua_pushstring(L, "v"); // {}, {}, "v"
    lua_setfield(L, -2, "__mode"); // {}, {}
    lua_setmetatable(L, -2); // {}
    evlua_registry = luaL_ref(L, LUA_REGISTRYINDEX); // <empty>
}
