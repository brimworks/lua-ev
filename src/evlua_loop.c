#include "private_evlua.h"
#include <lua.h>
#include <lauxlib.h>
#include <pthread.h>
#include <ev.h>
#include <string.h>

/**
 * Thread local storage for the lua_State object.
 */
static pthread_key_t  tls_L;
static pthread_once_t tls_init = PTHREAD_ONCE_INIT;
static void make_tls_L();

//////////////////////////////////////////////////////////////////////
// PUBLIC:
//////////////////////////////////////////////////////////////////////
/**
 * @see header
 */
lua_State* evlua_loop_state() {
    return (lua_State*)pthread_getspecific(tls_L);
}

/**
 * @see header
 */
void evlua_loop_set_state(lua_State* L) {
    (void)pthread_once(&tls_init, make_tls_L);
    pthread_setspecific(tls_L, L);
}

/**
 * @see header
 *
 * [-0, +0, m]
 */
void evlua_loop_ref(lua_State* L, int loop_idx, int watcher_idx, int is_daemon) {
    lua_getfenv(L, loop_idx); // {loop}
    lua_pushvalue(L, ( watcher_idx < 0 ? watcher_idx-1 : watcher_idx ));
    // {loop}, <watcher>
    lua_rawget(L, -2); // {loop}, bool
    int current_is_daemon = lua_toboolean(L, -1);
    lua_pop(L, 1); // {loop}

    if ( -1 == is_daemon ) is_daemon = current_is_daemon;
    if ( is_daemon ^ current_is_daemon ) {
        struct ev_loop* loop = loop_check(L, ( loop_idx < 0 ? loop_idx-1 : loop_idx ));
        if ( is_daemon ) {
            ev_unref(loop);
        } else {
            ev_ref(loop);
        }
    }
    lua_pushvalue(L, ( watcher_idx < 0 ? watcher_idx-1 : watcher_idx ));
    // {loop}, <watcher>
    lua_pushboolean(L, is_daemon); // {loop}, <watcher>, bool
    lua_rawset(L, -3); // {loop}
    lua_pop(L, 1); // <empty>
}

/**
 * @see header
 *
 * [-0, +0, m]
 */
void evlua_loop_unref(lua_State* L, int loop_idx, int watcher_idx) {
    lua_getfenv(L, loop_idx); // {loop}
    lua_pushvalue(L, ( watcher_idx < 0 ? watcher_idx-1 : watcher_idx ));
    // {loop}, <watcher>
    lua_rawget(L, -2); // {loop}, bool
    int is_daemon = lua_toboolean(L, -1);
    lua_pop(L, 1); // {loop}
    if ( is_daemon ) {
        struct ev_loop* loop =
            loop_check(L, ( loop_idx < 0 ? loop_idx-1 : loop_idx ));
        ev_ref(loop);
    }
    lua_pushvalue(L, ( watcher_idx < 0 ? watcher_idx-1 : watcher_idx ));
    // {loop}, <watcher>
    lua_pushnil(L); // {loop}, <watcher>, nil
    lua_rawset(L, -3); // {loop}
    lua_pop(L, 1); // <empty>
}

//////////////////////////////////////////////////////////////////////
// PRIVATE:
//////////////////////////////////////////////////////////////////////

/**
 * Used for initializing our thread local key used to keep track of
 * the lua_State when we enter into the event loop.
 */
static void make_tls_L() {
    (void)pthread_key_create(&tls_L, NULL);
}

/**
 * Create a table intended as the loop object, sets the metatable,
 * registers it, creates the evlua_loop struct appropriately, and sets
 * the userdata metatable.
 *
 * [-0, +1, v]
 */
static struct ev_loop** loop_init(lua_State *L) {
    struct ev_loop **loop = (struct ev_loop**)
        lua_newuserdata(L, sizeof(struct ev_loop*)); // ud

    // Set the metatable for the userdata:
    luaL_getmetatable(L, EVLUA_LOOP); // ud, {meta}
    lua_setmetatable(L, -2);          // ud

    // Set the fenv for the userdata:
    lua_createtable(L, 0, 0); // ud, {}
    if ( ! lua_setfenv(L, -2) ) { // ud
        luaL_error(L, "InternalError: setfenv() failed at %s line %d", __FILE__, __LINE__);
    }

    return loop;
}

/**
 * Create a new non-default loop instance.
 */
static int loop_new(lua_State *L) {
    *(loop_init(L)) = ev_loop_new(EVFLAG_AUTO);
    return 1;
}

/**
 * Delete a loop instances.
 */
static int loop_delete(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1);

    // Never destroy default loop:
    if ( ev_is_default_loop(loop) ) return 0;

    ev_loop_destroy(loop);
    evlua_reg_delete(L);

    return 0;
}

/**
 * Check if this is the default event loop.
 */
static int loop_is_default(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1);
    lua_pushboolean(L, ev_is_default_loop(loop));
    return 1;
}

/**
 * How many times have we iterated though the event loop?
 */
static int loop_count(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1);
    lua_pushinteger(L, ev_loop_count(loop));
    return 1;
}

/**
 * The current event loop time.
 */
static int loop_now(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1);
    lua_pushnumber(L, ev_now(loop));
    return 1;
}

/**
 * Sync the event loop time with "real" time and return the "real"
 * time.
 */
static int loop_update_now(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1);
    ev_now_update(loop);
    lua_pushnumber(L, ev_now(loop));
    return 1;
}

/**
 * Actually do the event loop.
 */
static int loop_loop(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1);

    evlua_loop_set_state(L);
    ev_loop(loop, 0);

    return 0;
}

/**
 * "Quit" out of the event loop.
 */
static int loop_unloop(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1);
    ev_unloop(loop, EVUNLOOP_ALL);
    return 0;
}

/**
 * Create the evlua.loop metatable.
 */
static void create_evlua_loop(lua_State *L) {
    luaL_newmetatable(L, EVLUA_LOOP);

    lua_pushvalue(L, -1); // {1}, {1}
    lua_setfield(L, -2, "__index"); // {1}

    static luaL_reg m_loop_fn[] = {
        { "is_default", loop_is_default },
        { "count",      loop_count },
        { "now",        loop_now },
        { "update_now", loop_update_now },
        { "loop",       loop_loop },
        { "unloop",     loop_unloop },
        { "__gc",       loop_delete },
        { NULL, NULL }
    };
    luaL_register(L, NULL, m_loop_fn); // {1}
    lua_pop(L, 1); // <empty>
}

/**
 * Create the table that will be registered as evlua.loop (aka the
 * loop static methods).
 */
void evlua_open_loop(lua_State *L) {
    create_evlua_loop(L);

    // Static methods:
    lua_createtable(L, 0, 2);
    lua_pushcfunction(L, loop_new);
    lua_setfield(L, -2, "new");

    struct ev_loop** loop = loop_init(L);
    *loop = ev_default_loop(EVFLAG_AUTO);

    lua_setfield(L, -2, "default");
}
