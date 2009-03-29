#include "private_evlua.h"

#include <ev.h>
#include <lua.h>
#include <lauxlib.h>
#include <pthread.h>
#include <assert.h>

static int version(lua_State *L) {
    lua_pushnumber(L, ev_version_major());
    lua_pushnumber(L, ev_version_minor());
    return 2;
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

    // Create the table we return:
    lua_createtable(L, 0, 5);

    // Initialize the shared stuff:
    evlua_open_shared(L);

    // Exported classes:
    evlua_open_loop(L);
    lua_setfield(L, -2, "Loop");

    evlua_open_timer(L);
    lua_setfield(L, -2, "Timer");

    evlua_open_io(L);
    lua_setfield(L, -2, "IO");

    // Exported functions:
    lua_pushcfunction(L, version);
    lua_setfield(L, -2, "version");

    lua_pushcfunction(L, evlua_obj_count);
    lua_setfield(L, -2, "object_count");

    return 1;
}
