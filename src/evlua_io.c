#include "private_evlua.h"
#include <lua.h>
#include <lauxlib.h>
#include <string.h>

static void io_cb(struct ev_loop *loop, ev_io *io, int revents) {
    evlua_cb(loop, (int)io->data, revents, ev_is_active(io));
}

// io = IO.new(FUNC, FD, READ|WRITE)
//
// Create a new io object with FUNC callback that will trigger
// AFTER seconds then trigger every REPEAT seconds there-after.
static int io_new(lua_State *L) {
    luaL_checktype(L, 1, LUA_TFUNCTION);

    int fd     = luaL_checkint(L, 2);
    int events = luaL_checkint(L, 3);

    int ref;
    struct ev_io* io = (struct ev_io*)
        evlua_obj_init(L, sizeof(struct ev_io), EVLUA_IO, &ref);
    // {io}
    lua_pushvalue(L, 1);
    // {io}, func
    lua_rawseti(L, -2, EVLUA_WATCHER_FUNCTION);
    // {io}
    io->data = (void*)ref;

    ev_io_init(io,  &io_cb, fd, events);

    return 1;
}

// io:stop(loop)
static int io_stop(lua_State *L) {
    struct ev_io* io     = io_check(L, 1);
    struct ev_loop* loop = loop_check(L, 2)->obj;

    ev_io_stop(loop, io);
    evlua_loop_unref(L, 2, 1);

    return 0;
}

// io:start(loop [, is_daemon])
static int io_start(lua_State *L) {
    struct ev_io* io     = io_check(L, 1);
    struct ev_loop* loop = loop_check(L, 2)->obj;
    int is_daemon        = lua_toboolean(L, 3);

    if ( ! lua_isboolean(L, 3) ) is_daemon = -1;

    ev_io_start(loop, io);
    evlua_loop_ref(L, 2, 1, is_daemon);

    return 0;
}

// bool = io:is_active()
static int io_is_active(lua_State *L) {
    struct ev_io* io = io_check(L, 1);

    lua_pushboolean(L, ev_is_active(io));

    return 1;
}

// bool = io:is_pending()
static int io_is_pending(lua_State *L) {
    struct ev_io* io = io_check(L, 1);

    lua_pushboolean(L, ev_is_pending(io));

    return 1;
}

// revents = io:clear_pending(loop)
static int io_clear_pending(lua_State *L) {
    struct ev_io* io     = io_check(L, 1);
    struct ev_loop* loop = loop_check(L, 2)->obj;

    int revents = ev_clear_pending(loop, io);
    if ( ! ev_is_active(io) ) evlua_loop_unref(L, 2, 1);

    lua_pushnumber(L, revents);
    return 1;
}

static int io_delete(lua_State *L) {
    evlua_obj_delete(L);
    return 0;
}

// old_callback = io:callback([new_callback])
int io_callback(lua_State *L) {
    io_check(L, 1);
    int n = lua_gettop(L);
    lua_rawgeti(L, 1, EVLUA_WATCHER_FUNCTION);
    if ( n > 1 ) {
        luaL_checktype(L, 2, LUA_TFUNCTION);
        lua_pushvalue(L, 2);
        lua_rawseti(L, 1, EVLUA_WATCHER_FUNCTION);
    }
    return 1;
}

/**
 * Create the evlua.io.ud metatable.
 */
static void create_evlua_io_ud(lua_State *L) {
    // userdata metatable:
    luaL_newmetatable(L, EVLUA_IO); // {}

    lua_pushcfunction(L, io_delete); // {}, func
    lua_setfield(L, -2, "__gc");  // {}

    lua_pop(L, 1); // <empty>
}

/**
 * Create the evlua.io metatable.
 */
static void create_evlua_io(lua_State *L) {
    // io metatable:
    lua_createtable(L, 0, 0); // {1}
    lua_pushlstring(L, EVLUA_IO, strlen(EVLUA_IO)-3); // {1}, "io"
    lua_pushvalue(L, -2); // {1}, "io", {1}
    lua_rawset(L, LUA_REGISTRYINDEX); // {1}

    lua_pushvalue(L, -1); // {1}, {1}
    lua_setfield(L, -2, "__index"); // {1}

    static luaL_reg m_io_fn[] = {
        { "stop",          io_stop },
        { "start",         io_start },
        { "is_active",     io_is_active },
        { "is_pending",    io_is_pending },
        { "clear_pending", io_clear_pending },
        { "callback",      io_callback },
        { NULL, NULL }
    };
    luaL_register(L, NULL, m_io_fn); // {1}
    lua_pop(L, 1); // <empty>
}

// Returns a table to be registered by evlua.io
void evlua_open_io(lua_State *L) {
    create_evlua_io_ud(L);
    create_evlua_io(L);

    // At this point only one static method:
    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, io_new);
    lua_setfield(L, -2, "new");
}
