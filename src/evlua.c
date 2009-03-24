#include "private_evlua.h"

#include <ev.h>
#include <lua.h>
#include <lauxlib.h>
#include <pthread.h>
#include <assert.h>

int evlua_registry=0;

static int version(lua_State *L) {
    lua_pushnumber(L,
                   ev_version_major() +
                   ( ev_version_minor() / 10000.0 ));
    return 1;
}

// module name (that argument is ignored).
LUALIB_API int luaopen_evlua(lua_State *L) {

    // Assert that we get an acceptable libev.so version:
    assert(ev_version_major() == EV_VERSION_MAJOR &&
           ev_version_minor() >= EV_VERSION_MINOR);

    // init the default event loop, register the special atfork func:
    ev_default_loop(EVFLAG_AUTO) ||
        luaL_error(L,
                   "libev init failed, perhaps LIBEV_FLAGS environment variable "
                   " is causing it to select a bad backend?");
    pthread_atfork(0, 0, ev_default_fork);

    // Create the global weak refs table which is simply a set of
    // evlua objects that exist.  The key into this table is an
    // integer (assigned via luaL_ref), and the value is a lua table
    // appropriate for the type of object.
    lua_newtable(L);
    lua_createtable(L, 0, 1);
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2);
    evlua_registry = luaL_ref(L, LUA_REGISTRYINDEX);

    // Create the table we return:
    lua_createtable(L, 0, 5);

    // Exported structures:
    assert(luaopen_evlua_loop(L) == 1);
    lua_setfield(L, -2, "loop");

    assert(luaopen_evlua_timer(L) == 1);
    lua_setfield(L, -2, "timer");

    assert(luaopen_evlua_io(L) == 1);
    lua_setfield(L, -2, "io");

    // Exported functions:
    lua_pushcfunction(L, version);
    lua_setfield(L, -2, "version");

    lua_pushcfunction(L, evlua_watcher_count);
    lua_setfield(L, -2, "watcher_count");

    return 1;
}
