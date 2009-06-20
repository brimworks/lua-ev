#include "private_evlua.h"
#include <lua.h>
#include <lauxlib.h>
#include <string.h>
#include <stddef.h>

static void timer_cb(struct ev_loop *loop, ev_timer *timer, int revents) {
    evlua_cb(loop, (int)timer->data, revents, ev_is_active(timer));
}

// TIMER = Timer.new(FUNC, AFTER [, REPEAT])
//
// Create a new timer object with FUNC callback that will trigger
// AFTER seconds then trigger every REPEAT seconds there-after.
static int timer_new(lua_State *L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);

    ev_tstamp after  = luaL_checknumber(L, 2);
    ev_tstamp repeat = luaL_optnumber(L, 3, 0);

    if ( after < 0 )
        luaL_argerror(L, 3, "after must be greater than 0");
    if ( repeat < 0 )
        luaL_argerror(L, 4, "repeat must be greater than 0");

    int ref;
    struct ev_timer* timer = (struct ev_timer*)
        evlua_watcher_new(L, sizeof(struct ev_timer), EVLUA_TIMER, &ref);
    // {timer}
    ev_timer_init(timer,  &timer_cb, after, repeat);
    timer->data = (void*)ref;

    return 1;
}

// timer:again(loop[, seconds])
static int timer_again(lua_State *L) {
    struct ev_timer* timer = timer_check(L, 1);
    struct ev_loop* loop   = loop_check(L, 2);
    ev_tstamp repeat       = luaL_optnumber(L, 3, 0);

    if ( repeat < 0 ) luaL_argerror(L, 1, "repeat must be greater than 0");

    if ( repeat ) timer->repeat = repeat;

    ev_timer_again(loop, timer);
    evlua_loop_ref(L, 2, 1, -1);

    return 0;
}

// timer:stop(loop)
static int timer_stop(lua_State *L) {
    struct ev_timer* timer = timer_check(L, 1);
    struct ev_loop* loop   = loop_check(L, 2);

    ev_timer_stop(loop, timer);
    evlua_loop_unref(L, 2, 1);

    return 0;
}

// timer:start(loop [, is_daemon])
static int timer_start(lua_State *L) {
    struct ev_timer* timer = timer_check(L, 1);
    struct ev_loop* loop   = loop_check(L, 2);
    int is_daemon          = lua_toboolean(L, 3);

    if ( ! lua_isboolean(L, 3) ) is_daemon = -1;

    ev_timer_start(loop, timer);
    evlua_loop_ref(L, 2, 1, is_daemon);

    return 0;
}

// bool = timer:is_active()
static int timer_is_active(lua_State *L) {
    struct ev_timer* timer = timer_check(L, 1);

    lua_pushboolean(L, ev_is_active(timer));

    return 1;
}

// bool = timer:is_pending()
static int timer_is_pending(lua_State *L) {
    struct ev_timer* timer = timer_check(L, 1);

    lua_pushboolean(L, ev_is_pending(timer));

    return 1;
}

// revents = timer:clear_pending(loop)
static int timer_clear_pending(lua_State *L) {
    struct ev_timer* timer = timer_check(L, 1);
    struct ev_loop* loop   = loop_check(L, 2);

    int revents = ev_clear_pending(loop, timer);
    if ( ! ev_is_active(timer) ) evlua_loop_unref(L, 2, 1);

    lua_pushnumber(L, revents);
    return 1;
}

static int timer_delete(lua_State *L) {
    evlua_reg_delete(L);
    return 0;
}

// old_callback = timer:callback([new_callback])
int timer_callback(lua_State *L) {
    int n = lua_gettop(L);
    timer_check(L, 1);
    lua_getfenv(L, 1); // {timer}
    lua_rawgeti(L, -1, EVLUA_WATCHER_FUNCTION); // {timer}, func
    if ( n > 1 ) {
        luaL_checktype(L, 2, LUA_TFUNCTION);
        lua_pushvalue(L, 2); // {timer}, func, new_func
        lua_rawseti(L, -3, EVLUA_WATCHER_FUNCTION);
    }
    return 1;
}

/**
 * Create the evlua.timer metatable.
 */
static void create_evlua_timer(lua_State *L) {
    // timer metatable:
    luaL_newmetatable(L, EVLUA_TIMER); // {}

    lua_pushvalue(L, -1); // {1}, {1}
    lua_setfield(L, -2, "__index"); // {1}

    static luaL_reg m_timer_fn[] = {
        { "again",         timer_again },
        { "stop",          timer_stop },
        { "start",         timer_start },
        { "is_active",     timer_is_active },
        { "is_pending",    timer_is_pending },
        { "clear_pending", timer_clear_pending },
        { "callback",      timer_callback },
        { "__gc",          timer_delete },
        { NULL, NULL }
    };
    luaL_register(L, NULL, m_timer_fn); // {1}
    lua_pop(L, 1); // <empty>
}

// Returns a table to be registered by evlua.timer
void evlua_open_timer(lua_State *L) {
    create_evlua_timer(L);

    // At this point only one static method:
    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, timer_new);
    lua_setfield(L, -2, "new");
}
