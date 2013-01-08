#include <assert.h>
#include <ev.h>
#include <lauxlib.h>
#include <lua.h>
#include <signal.h>

#include "lua_ev.h"

/* We make everything static, so we just include all *.c files in a
 * single compilation unit. */
#include "obj_lua_ev.c"
#include "loop_lua_ev.c"
#include "watcher_lua_ev.c"
#include "io_lua_ev.c"
#include "timer_lua_ev.c"
#include "signal_lua_ev.c"
#include "idle_lua_ev.c"
#include "child_lua_ev.c"
#include "stat_lua_ev.c"

static const luaL_reg R[] = {
    {"version", version},
    {"object_count", obj_count},
    {NULL, NULL},
};

/**
 * Entry point into the 'ev' lua library.  Validates that the
 * dynamically linked libev version matches, creates the object
 * registry, and creates the table returned by require().
 */
LUALIB_API int luaopen_ev(lua_State *L) {

    assert(ev_version_major() == EV_VERSION_MAJOR &&
           ev_version_minor() >= EV_VERSION_MINOR);

    create_obj_registry(L);

    luaL_register(L, "ev", R);

    luaopen_ev_loop(L);
    lua_setfield(L, -2, "Loop");

    luaopen_ev_timer(L);
    lua_setfield(L, -2, "Timer");

    luaopen_ev_io(L);
    lua_setfield(L, -2, "IO");

    luaopen_ev_signal(L);
    lua_setfield(L, -2, "Signal");

    luaopen_ev_idle(L);
    lua_setfield(L, -2, "Idle");

    luaopen_ev_child(L);
    lua_setfield(L, -2, "Child");

    luaopen_ev_stat(L);
    lua_setfield(L, -2, "Stat");

    lua_pushnumber(L, EV_READ);
    lua_setfield(L, -2, "READ");

    lua_pushnumber(L, EV_WRITE);
    lua_setfield(L, -2, "WRITE");

    lua_pushnumber(L, EV_TIMEOUT);
    lua_setfield(L, -2, "TIMEOUT");

    lua_pushnumber(L, EV_SIGNAL);
    lua_setfield(L, -2, "SIGNAL");

    lua_pushnumber(L, EV_IDLE);
    lua_setfield(L, -2, "IDLE");

    lua_pushnumber(L, EV_CHILD);
    lua_setfield(L, -2, "CHILD");

    lua_pushnumber(L, EV_STAT);
    lua_setfield(L, -2, "STAT");

    lua_pushnumber(L, EV_MINPRI);
    lua_setfield(L, -2, "MINPRI");

    lua_pushnumber(L, EV_MAXPRI);
    lua_setfield(L, -2, "MAXPRI");

    lua_pushnumber(L, SIGHUP);
    lua_setfield(L, -2, "SIGHUP");

    lua_pushnumber(L, SIGINT);
    lua_setfield(L, -2, "SIGINT");    

    lua_pushnumber(L, SIGQUIT);
    lua_setfield(L, -2, "SIGQUIT");    

    lua_pushnumber(L, SIGILL);
    lua_setfield(L, -2, "SIGILL");    

    lua_pushnumber(L, SIGTRAP);
    lua_setfield(L, -2, "SIGTRAP");    

    lua_pushnumber(L, SIGABRT);
    lua_setfield(L, -2, "SIGABRT");    

    lua_pushnumber(L, SIGIOT);
    lua_setfield(L, -2, "SIGIOT");    

    lua_pushnumber(L, SIGBUS);
    lua_setfield(L, -2, "SIGBUS");    

    lua_pushnumber(L, SIGFPE);
    lua_setfield(L, -2, "SIGFPE");    

    lua_pushnumber(L, SIGUSR1);
    lua_setfield(L, -2, "SIGUSR1");

    lua_pushnumber(L, SIGSEGV);
    lua_setfield(L, -2, "SIGSEGV");    

    lua_pushnumber(L, SIGUSR2);
    lua_setfield(L, -2, "SIGUSR2");    

    lua_pushnumber(L, SIGPIPE);
    lua_setfield(L, -2, "SIGPIPE");    

    lua_pushnumber(L, SIGALRM);
    lua_setfield(L, -2, "SIGALRM");    

    lua_pushnumber(L, SIGTERM);
    lua_setfield(L, -2, "SIGTERM");    

    lua_pushnumber(L, SIGSTKFLT);
    lua_setfield(L, -2, "SIGSTKFLT");    

    lua_pushnumber(L, SIGCHLD);
    lua_setfield(L, -2, "SIGCHLD");    

    lua_pushnumber(L, SIGCONT);
    lua_setfield(L, -2, "SIGCONT");    

    lua_pushnumber(L, SIGTSTP);
    lua_setfield(L, -2, "SIGTSTP");    

    lua_pushnumber(L, SIGTTIN);
    lua_setfield(L, -2, "SIGTTIN");    

    lua_pushnumber(L, SIGTTOU);
    lua_setfield(L, -2, "SIGTTOU");    

    lua_pushnumber(L, SIGURG);
    lua_setfield(L, -2, "SIGURG");    

    lua_pushnumber(L, SIGXCPU);
    lua_setfield(L, -2, "SIGXCPU");    

    lua_pushnumber(L, SIGXFSZ);
    lua_setfield(L, -2, "SIGXFSZ");    

    lua_pushnumber(L, SIGVTALRM);
    lua_setfield(L, -2, "SIGVTALRM");    

    lua_pushnumber(L, SIGPROF);
    lua_setfield(L, -2, "SIGPROF");    

    lua_pushnumber(L, SIGWINCH);
    lua_setfield(L, -2, "SIGWINCH");    

    lua_pushnumber(L, SIGIO);
    lua_setfield(L, -2, "SIGIO");    

    lua_pushnumber(L, SIGPOLL);
    lua_setfield(L, -2, "SIGPOLL");    

    lua_pushnumber(L, SIGPWR);
    lua_setfield(L, -2, "SIGPWR");    

    lua_pushnumber(L, SIGSYS);
    lua_setfield(L, -2, "SIGSYS");    

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
