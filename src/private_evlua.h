#ifndef _PRIVATE_EVLUA_H
#define _PRIVATE_EVLUA_H

#include <lua.h>
#include <ev.h>

#define EVLUA_LOOP  "evlua.loop"
#define EVLUA_TIMER "evlua.timer"
#define EVLUA_IO    "evlua.io"
#define EVLUA_WATCHER_FUNCTION 1

/**
 * Various wrappers to luaL_checkudata:
 */
#define loop_check(L, narg) \
    (*((struct ev_loop**)luaL_checkudata((L), (narg), EVLUA_LOOP)))
#define timer_check(L, narg) \
    ((struct ev_timer*)luaL_checkudata((L), (narg), EVLUA_TIMER))
#define io_check(L, narg) \
    ((struct ev_io*)luaL_checkudata((L), (narg), EVLUA_IO))

/**
 * Initialize various submodules.
 *
 * [-0, +1, e]
 */
void evlua_open_timer(lua_State *L);
void evlua_open_io(lua_State *L);
void evlua_open_loop(lua_State *L);
// [-0, +0, e]
void evlua_open_shared(lua_State *L);


/**
 * Look inside and see how many objects are registed in the registry.
 * This is used so we can assert that garbage is properly collected in
 * our unit tests.
 *
 * [-0, +1, -]
 */
int evlua_reg_count(lua_State *L);

/**
 * Retrieve the object referred to by ref from the registry.
 *
 * @param L The lua_State
 * @param ref The value assigned to on evlua_reg_init.
 *
 * [-0, +1, v]
 */
void evlua_reg_get(lua_State* L, int ref);

/**
 * Create a table that is registered in the evlua_registry.  This
 * table will contain a userdata of <size> in the LUA_NOREF index of
 * this table.  The metatable of the userdata and table will both be
 * set.  The resultant table is left on the stack, the userdata is
 * returned as a void* and *ref is assigned to be the index in the
 * evlua_registry of the table.
 *
 * @param L The lua_State
 * @param c_type The C type we are creating.
 * @param lua_type The string name of the metatable name.
 * @param ref Assigned to the location in the registry.
 *
 * [-0, +1, v]
 */
void* evlua_watcher_new(lua_State *L, size_t c_type_size, char* lua_type, int* ref);

/**
 * Call when an object is destroyed.
 */
void evlua_reg_delete(lua_State* L);

/**
 * Add a reference to watcher from loop.  Mark this as a "daemon"
 * watcher if you don't want the watcher to prevent the event loop
 * from terminating.
 *
 * @param loop_idx the index of the loop table.
 * @param watcher_idx the index of the watcher table.
 * @param is_daemon true, false or -1 to indicate that any current
 *     value for the daemon flag should be preserved.
 *
 * [-0, +0, m]
 */
void evlua_loop_ref(lua_State* L, int loop_idx, int watcher_idx, int is_daemon);

/**
 * Remove a reference to watcher from loop.
 *
 * [-0, +0, m]
 */
void evlua_loop_unref(lua_State* L, int loop_idx, int watcher_idx);

/**
 * Generic callback for all watchers.  Expects <loop> to be on the top
 * of the stack!
 *
 * [-0, +0, m]
 */
void evlua_cb(struct ev_loop *loop, int watcher_ref, int revents, int is_active);

/**
 * Get the thread local lua_State that was set when the event loop was started.
 */
lua_State* evlua_loop_state();

#endif /* PRIVATE_EVLUA */
