/**
 * Create a table for ev.Async that gives access to the constructor for
 * async objects.
 *
 * [-0, +1, ?]
 */
static int luaopen_ev_async(lua_State *L) {
    lua_pop(L, create_async_mt(L));

    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, async_new);
    lua_setfield(L, -2, "new");

    lua_pushcfunction(L, async_export);
    lua_setfield(L, -2, "export");

    lua_pushcfunction(L, async_import);
    lua_setfield(L, -2, "import");

    return 1;
}

/**
 * Create the async metatable in the registry.
 *
 * [-0, +1, ?]
 */
static int create_async_mt(lua_State *L) {

    static luaL_Reg fns[] = {
        { "send",          async_send },
        { "stop",          async_stop },
        { "start",         async_start },
        { NULL, NULL }
    };
    luaL_newmetatable(L, ASYNC_MT);
    add_watcher_mt(L);
    luaL_setfuncs(L, fns, 0);

    return 1;
}

/**
 * Create a new async object.  Arguments:
 *   1 - callback function.
 *
 * @see watcher_new()
 *
 * [+1, -0, ?]
 */
static int async_new(lua_State* L) {
    ev_async*  async;

    async = watcher_new(L, sizeof(ev_async), ASYNC_MT);
    ev_async_init(async, &async_cb );
    return 1;
}

struct async_serialized {
  ev_async*       async;
  struct ev_loop* loop;
};

static int async_sz_send(lua_State *L) {
    void *udata = lua_touserdata(L, lua_upvalueindex(1));
    struct async_serialized* data = (struct async_serialized*)udata;

    ev_async_send(data->loop, data->async);

    return 0;
}

/**
 * Export serialized send for multi-threading signalization (low-level API).
 *
 * Return string.
 *
 * Usage:
 *     string = Async.export(async, loop)
 *
 * [+1, -0, ?]
 */
static int async_export(lua_State *L) {
    ev_async*       async  = check_async(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);

    struct async_serialized data;
    data.async = async;
    data.loop  = loop;
    lua_pushlstring(L, (const char*)&data, sizeof(struct async_serialized));

    return 1;
}

/**
 * Import serialized send for multi-threading signalization (low-level API).
 *
 * Return a function which will act as async:send(loop) when called.
 *
 * Calling the function beyond the lifespan of either the exported loop or
 * async object will result in undefined behavior (crash/corruption). The same
 * applies with an ill-formed string.
 *
 * Usage:
 *     async_send = Async.import(string)
 *
 * [+1, -0, ?]
 */
static int async_import(lua_State *L) {
    size_t len;
    const char* str = luaL_checklstring(L, 1, &len);
    if ( len != sizeof(struct async_serialized) )
      luaL_error(L, "invalid data size");

    void* data = lua_newuserdata(L, sizeof(struct async_serialized));
    memcpy(data, str, sizeof(struct async_serialized));
    lua_pushcclosure(L, async_sz_send, 1);

    return 1;
}

/**
 * @see watcher_cb()
 *
 * [+0, -0, m]
 */
static void async_cb(struct ev_loop* loop, ev_async* async, int revents) {
    watcher_cb(loop, async, revents);
}

/**
 * Sends/signals/activates the given "ev_async" watcher, that is, feeds an
 * "EV_ASYNC" event on the watcher into the event loop, and instantly returns.
 *
 * Usage:
 *     async:send(loop)
 *
 * [+0, -0, e]
 */
static int async_send(lua_State *L) {
    ev_async*       async  = check_async(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);

    ev_async_send(loop, async);

    return 0;
}

/**
 * Stops the async so it won't be called by the specified event loop.
 *
 * Usage:
 *     async:stop(loop)
 *
 * [+0, -0, e]
 */
static int async_stop(lua_State *L) {
    ev_async*       async  = check_async(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);

    loop_stop_watcher(L, 2, 1);
    ev_async_stop(loop, async);

    return 0;
}

/**
 * Starts the async so it won't be called by the specified event loop.
 *
 * Usage:
 *     async:start(loop [, is_daemon])
 *
 * [+0, -0, e]
 */
static int async_start(lua_State *L) {
    ev_async*       async  = check_async(L, 1);
    struct ev_loop* loop   = *check_loop_and_init(L, 2);
    int is_daemon          = lua_toboolean(L, 3);

    ev_async_start(loop, async);
    loop_start_watcher(L, 2, 1, is_daemon);

    return 0;
}
