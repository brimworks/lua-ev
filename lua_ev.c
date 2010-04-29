#include <assert.h>
#include <ev.h>
#include <lauxlib.h>
#include <lua.h>
#include <pthread.h>

#include "lua_ev.h"

/* We make everything static, so we just include all *.c files in a
 * single compilation unit. */
#include "obj_lua_ev.c"
#include "loop_lua_ev.c"
#include "watcher_lua_ev.c"
#include "io_lua_ev.c"
#include "timer_lua_ev.c"
#include "signal_lua_ev.c"

/**
 * Entry point into the 'ev' lua library.  Validates that the
 * dynamically linked libev version matches, creates the object
 * registry, and creates the table returned by require().
 */
LUALIB_API int luaopen_ev(lua_State *L) {

    assert(ev_version_major() == EV_VERSION_MAJOR &&
           ev_version_minor() >= EV_VERSION_MINOR);

    create_obj_registry(L);

    lua_createtable(L, 0, 10);

    luaopen_ev_loop(L);
    lua_setfield(L, -2, "Loop");

    luaopen_ev_timer(L);
    lua_setfield(L, -2, "Timer");

    luaopen_ev_io(L);
    lua_setfield(L, -2, "IO");

    luaopen_ev_signal(L);
    lua_setfield(L, -2, "Signal");

    lua_pushcfunction(L, version);
    lua_setfield(L, -2, "version");

    lua_pushcfunction(L, obj_count);
    lua_setfield(L, -2, "object_count");

    lua_pushnumber(L, EV_READ);
    lua_setfield(L, -2, "READ");

    lua_pushnumber(L, EV_WRITE);
    lua_setfield(L, -2, "WRITE");

    lua_pushnumber(L, EV_TIMEOUT);
    lua_setfield(L, -2, "TIMEOUT");

    lua_pushnumber(L, EV_SIGNAL);
    lua_setfield(L, -2, "SIGNAL");

    return 1;
}

/**
 * Push the major and minor version of libev onto the stack.
 *
 * [+2, -0, -]
 */
static int version(lua_State *L) {
    lua_pushnumber(L, ev_version_major());
    lua_pushnumber(L, ev_version_minor());
    return 2;
}

/**
 * Taken from lua.c out of the lua source distribution.  Use this
 * function when doing lua_pcall().
 */
static int traceback(lua_State *L) {
    if ( !lua_isstring(L, 1) ) return 1;

    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if ( !lua_istable(L, -1) ) {
        lua_pop(L, 1);
        return 1;
    }

    lua_getfield(L, -1, "traceback");
    if ( !lua_isfunction(L, -1) ) {
        lua_pop(L, 2);
        return 1;
    }

    lua_pushvalue(L, 1);    /* pass error message */
    lua_pushinteger(L, 2);  /* skip this function and traceback */
    lua_call(L, 2, 1);      /* call debug.traceback */
    return 1;
}
