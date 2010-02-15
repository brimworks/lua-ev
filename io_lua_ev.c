/**
 * Create a table for ev.IO that gives access to the constructor for
 * io objects.
 *
 * [-0, +1, ?]
 */
static int luaopen_ev_io(lua_State *L) {
    lua_pop(L, create_io_mt(L));

    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, io_new);
    lua_setfield(L, -2, "new");

    return 1;
}

/**
 * Create the io metatable in the registry.
 *
 * [-0, +1, ?]
 */
static int create_io_mt(lua_State *L) {

    static luaL_reg fns[] = {
        { "stop",          io_stop },
        { "start",         io_start },
        { "is_active",     io_is_active },
        { "is_pending",    io_is_pending },
        { "clear_pending", io_clear_pending },
        { "callback",      io_callback },
        { NULL, NULL }
    };
    luaL_newmetatable(L, IO_MT);
    luaL_register(L, NULL, fns);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    return 1;
}

/**
 * Create a new io object.  Arguments:
 *   1 - callback function.
 *   2 - fd (file descriptor number)
 *   3 - READ | WRITE (what operation to watch)
 *
 * @see watcher_new()
 *
 * [+1, -0, ?]
 */
static int io_new(lua_State* L) {
    int     fd     = luaL_checkint(L, 2);
    int     events = luaL_checkint(L, 3);
    ev_io*  io;

    io = watcher_new(L,
                     sizeof(ev_io),
                     IO_MT,
                     offsetof(ev_io, data));
    ev_io_init(io, &io_cb, fd, events);
    return 1;
}

/**
 * @see watcher_cb()
 *
 * [+0, -0, m]
 */
static void io_cb(struct ev_loop* loop, ev_io* io, int revents) {
    watcher_cb(io->data, loop, io, revents);
}

/**
 * Stops the io so it won't be called by the specified event loop.
 *
 * Usage:
 *     io:stop(loop)
 *
 * [+0, -0, e]
 */
static int io_stop(lua_State *L) {
    ev_io*          io     = check_io(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);

    loop_stop_watcher(L, 2, 1);
    ev_io_stop(loop, io);

    return 0;
}

/**
 * Starts the io so it won't be called by the specified event loop.
 *
 * Usage:
 *     io:start(loop [, is_daemon])
 *
 * [+0, -0, e]
 */
static int io_start(lua_State *L) {
    ev_io*       io  = check_io(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);
    int is_daemon          = lua_toboolean(L, 3);

    ev_io_start(loop, io);
    loop_start_watcher(L, 2, 1, is_daemon);

    return 0;
}

/**
 * Test if the io is active.
 *
 * Usage:
 *   bool = io:is_active()
 *
 * [+1, -0, e]
 */
static int io_is_active(lua_State *L) {
    lua_pushboolean(L, ev_is_active(check_io(L, 1)));
    return 1;
}

/**
 * Test if the io is pending.
 *
 * Usage:
 *   bool = io:is_pending()
 *
 * [+1, -0, e]
 */
static int io_is_pending(lua_State *L) {
    lua_pushboolean(L, ev_is_pending(check_io(L, 1)));
    return 1;
}

/**
 * If the io is pending, return the revents and clear the pending
 * status (so the io callback won't be called).
 *
 * Usage:
 *   revents = io:clear_pending(loop)
 *
 * [+1, -0, e]
 */
static int io_clear_pending(lua_State *L) {
    lua_pushnumber(L, ev_clear_pending(*check_loop_and_init(L, 2), check_io(L, 1)));
    return 1;
}

/**
 * @see watcher_callback()
 *
 * [+1, -0, e]
 */
static int io_callback(lua_State *L) {
    return watcher_callback(L, IO_MT);
}
