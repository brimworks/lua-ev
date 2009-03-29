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
    evlua_obj_get(L, watcher_ref); // {loop}, {watcher}
    if ( lua_isnil(L, -1) ) {
        lua_pushfstring(L,
                        "InternalError: event callback executed but is missing the 'self' field at %s line %d\n",
                        __FILE__, __LINE__);
        // {loop}, {watcher}, ERR
        goto ERROR;
    }
    if ( ! is_active ) evlua_loop_unref(L, -2, -1);
    lua_rawgeti(L, -1, EVLUA_WATCHER_FUNCTION);
    // {loop}, {watcher}, callback
    if ( lua_isnil(L, -1) ) {
        lua_pop(L, 1);
        // {loop}, {watcher}

        // TODO: Handle OOM...
        lua_pushfstring(L,
                        "MissingCallback: Internal field of an event was modified so the callback function is missing at %s line %d\n",
                        __FILE__, __LINE__);
        // {loop}, {watcher}, ERR
        goto ERROR;
    }

    lua_pushvalue(L, -3);
    // {loop}, {watcher}, callback, {loop}
    lua_pushvalue(L, -3);
    // {loop}, {watcher}, callback, {loop}, {watcher}
    lua_pushinteger(L, revents);
    // {loop}, {watcher}, callback, {loop}, {watcher}, revents

    // The show must go on, so ignore any error.
    int err = lua_pcall(L, 3, 0, 0);
    // {loop}, {watcher} [, ERR]
    if ( err ) {
      ERROR:
        // {loop}, {watcher}, ERR

        // TODO: add low level debug statement.
        fprintf(stderr, "FAILED CALLBACK: %s\n",
                lua_tostring(L, -1));

        lua_pop(L, 1);
        // {loop}, {watcher}
    }
    lua_pop(L, 1); // {loop}
}

void evlua_obj_delete(lua_State* L) {
    --evlua_live_objects;
}

//////////////////////////////////////////////////////////////////////
int evlua_obj_count(lua_State *L) {
    lua_pushinteger(L, evlua_live_objects);
    return 1;
}

//////////////////////////////////////////////////////////////////////
void evlua_obj_get(lua_State* L, int ref) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, evlua_registry); // {reg}
    lua_rawgeti(L, -1, ref); // {reg}, {obj}
    lua_remove(L, -2); // {obj}
}

//////////////////////////////////////////////////////////////////////
void* evlua_obj_init(lua_State *L, size_t size, char* type_userdata, int* ref) {
    // Create table, set metatable:
    lua_createtable(L, 0, 1);   // {}

    lua_pushlstring(L, type_userdata, strlen(type_userdata)-3); // {}, "type"
    lua_rawget(L, LUA_REGISTRYINDEX); // {}, {meta}
    lua_setmetatable(L, -2);    // {}

    // Register the table:
    lua_rawgeti(L, LUA_REGISTRYINDEX, evlua_registry); // {}, {reg}
    lua_pushvalue(L, -2); // {}, {reg}, {}
    *ref = luaL_ref(L, -2); // {}, {reg}
    lua_pop(L, 1); // {}
    evlua_live_objects++;

    // Create the object:
    void *obj = lua_newuserdata(L, size); // {}, ud

    // Set the metatable for the userdata:
    luaL_getmetatable(L, type_userdata); // {}, ud, {evlua.loop.ud}
    lua_setmetatable(L, -2); // {}, ud
    lua_rawseti(L, -2, LUA_NOREF); // {}

    return obj;
}

//////////////////////////////////////////////////////////////////////
void* evlua_obj_check(lua_State *L, int ud, char* type_userdata) {
    luaL_checktype(L, ud, LUA_TTABLE);
    lua_rawgeti(L, ud, LUA_NOREF); // {watcher}
    void *p = lua_touserdata(L, -1);
    if ( p != NULL ) {
        if ( lua_getmetatable(L, -1) ) {
            // {watcher}, {w.meta}
            lua_getfield(L, LUA_REGISTRYINDEX, type_userdata);
            // {watcher}, {w.meta}, {r.meta}
            if ( lua_rawequal(L, -1, -2) ) {
                lua_pop(L, 3);
                return p;
            }
        }
    }
    const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                      type_userdata, luaL_typename(L, -1));
    luaL_argerror(L, ud, msg);
    return NULL;
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
