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
    lua_pushvalue(L, watcher_idx); // {watcher}
    lua_rawget(L, ( loop_idx < 0 ? loop_idx-1 : loop_idx )); // bool
    int current_is_daemon = lua_toboolean(L, -1);
    lua_pop(L, 1); // <empty>

    if ( -1 == is_daemon ) is_daemon = current_is_daemon;
    if ( is_daemon ^ current_is_daemon ) {
        struct ev_loop* loop = loop_check(L, loop_idx)->obj;
        if ( is_daemon ) {
            ev_unref(loop);
        } else {
            ev_ref(loop);
        }
    }
    lua_pushvalue(L, watcher_idx); // {watcher}
    lua_pushboolean(L, is_daemon); // {watcher}, bool
    lua_rawset(L, ( loop_idx < 0 ? loop_idx-2 : loop_idx )); // <empty>
}

/**
 * @see header
 *
 * [-0, +0, m]
 */
void evlua_loop_unref(lua_State* L, int loop_idx, int watcher_idx) {
    lua_pushvalue(L, watcher_idx); // {watcher}
    lua_rawget(L, ( loop_idx < 0 ? loop_idx-1 : loop_idx )); // bool
    int is_daemon = lua_toboolean(L, -1);
    lua_pop(L, 1); // <empty>
    if ( is_daemon ) {
        struct ev_loop* loop = loop_check(L, loop_idx)->obj;
        ev_ref(loop);
    }
    lua_pushvalue(L, watcher_idx); // {watcher}
    lua_pushnil(L); // {watcher}, nil
    lua_rawset(L, ( loop_idx < 0 ? loop_idx-2 : loop_idx )); // <empty>
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
static struct evlua_loop* loop_init(lua_State *L) {
    int ref;
    struct evlua_loop* loop = (struct evlua_loop*)
        evlua_obj_init(L, sizeof(struct evlua_loop), EVLUA_LOOP, &ref);
    loop->obj = NULL;
    loop->ref = ref;
    return loop;
}

/**
 * Create a new non-default loop instance.
 */
static int loop_new(lua_State *L) {
    loop_init(L)->obj = ev_loop_new(EVFLAG_AUTO);
    return 1;
}

/**
 * Delete a loop instances.
 */
static int loop_delete(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1)->obj;

    // Never destroy default loop:
    if ( ev_is_default_loop(loop) ) return 0;

    ev_loop_destroy(loop);
    evlua_obj_delete(L);

    return 0;
}

/**
 * Check if this is the default event loop.
 */
static int loop_is_default(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1)->obj;
    lua_pushboolean(L, ev_is_default_loop(loop));
    return 1;
}

/**
 * How many times have we iterated though the event loop?
 */
static int loop_count(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1)->obj;
    lua_pushinteger(L, ev_loop_count(loop));
    return 1;
}

/**
 * The current event loop time.
 */
static int loop_now(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1)->obj;
    lua_pushnumber(L, ev_now(loop));
    return 1;
}

/**
 * Sync the event loop time with "real" time and return the "real"
 * time.
 */
static int loop_update_now(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1)->obj;
    ev_now_update(loop);
    lua_pushnumber(L, ev_now(loop));
    return 1;
}

/**
 * Actually do the event loop.
 */
static int loop_loop(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1)->obj;

    evlua_loop_set_state(L);
    ev_loop(loop, 0);

    return 0;
}

/**
 * "Quit" out of the event loop.
 */
static int loop_unloop(lua_State *L) {
    struct ev_loop* loop = loop_check(L, 1)->obj;
    ev_unloop(loop, EVUNLOOP_ALL);
    return 0;
}

/**
 * Create the evlua.loop.ud metatable.
 */
static void create_evlua_loop_ud(lua_State *L) {
    // userdata metatable:
    luaL_newmetatable(L, EVLUA_LOOP);

    lua_pushcfunction(L, loop_delete);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);
}

/**
 * Create the evlua.loop metatable.
 */
static void create_evlua_loop(lua_State *L) {
    // loop metatable:
    lua_createtable(L, 0, 0); // {1}
    lua_pushlstring(L, EVLUA_LOOP, strlen(EVLUA_LOOP)-3); // {1}, "loop"
    lua_pushvalue(L, -2); // {1}, "loop", {1}
    lua_rawset(L, LUA_REGISTRYINDEX); // {1}

    lua_pushvalue(L, -1); // {1}, {1}
    lua_setfield(L, -2, "__index"); // {1}

    static luaL_reg m_loop_fn[] = {
        { "is_default", loop_is_default },
        { "count",      loop_count },
        { "now",        loop_now },
        { "update_now", loop_update_now },
        { "loop",       loop_loop },
        { "unloop",     loop_unloop },
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
    create_evlua_loop_ud(L);
    create_evlua_loop(L);

    // Static methods:
    lua_createtable(L, 0, 2);
    lua_pushcfunction(L, loop_new);
    lua_setfield(L, -2, "new");

    loop_init(L)->obj = ev_default_loop(EVFLAG_AUTO);
    lua_setfield(L, -2, "default");
}
