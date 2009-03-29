#ifndef _PRIVATE_EVLUA_H
#define _PRIVATE_EVLUA_H

#include <lua.h>
#include <ev.h>

#define EVLUA_LOOP  "evlua.loop.ud"
#define EVLUA_TIMER "evlua.timer.ud"
#define EVLUA_IO    "evlua.io.ud"
#define EVLUA_WATCHER_FUNCTION 1

/**
 * Various wrappers to evlua_obj_check:
 */
#define loop_check(L, narg) \
    ((struct evlua_loop*)evlua_obj_check((L), (narg), EVLUA_LOOP))
#define timer_check(L, narg) \
    ((struct ev_timer*)evlua_obj_check((L), (narg), EVLUA_TIMER))
#define io_check(L, narg) \
    ((struct ev_io*)evlua_obj_check((L), (narg), EVLUA_IO))

/**
 * Wrap the ev_loop object so we can resolve back to the lua table
 * that backs this object.
 */
struct evlua_loop {
    struct ev_loop* obj;
    int             ref;
};


/**
 * Here is a high level overview of how evlua integrates libev with
 * lua:
 *
 * All objects are represented as tables with a LUA_NOREF element that
 * is a userdata for the underlying C structure.
 *
 * loop: Keeps a reference to all active watchers so they don't get
 *    GC'ed.
 *
 * watcher: lua table with:
 *    1 - the callback function.
 */

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
int evlua_obj_count(lua_State *L);

/**
 * Retrieve the object referred to by ref from the registry.
 *
 * @param L The lua_State
 * @param ref The value assigned to on evlua_obj_init.
 *
 * [-0, +1, v]
 */
void evlua_obj_get(lua_State* L, int ref);

/**
 * Create a table that is registered in the evlua_registry.  This
 * table will contain a userdata of <size> in the LUA_NOREF index of
 * this table.  The metatable of the userdata and table will both be
 * set.  The resultant table is left on the stack, the userdata is
 * returned as a void* and *ref is assigned to be the index in the
 * evlua_registry of the table.
 *
 * @param L The lua_State
 * @param size The amount of space needed for the user data.
 * @param type_userdata The metatable of the userdata, it must end in
 *      ".ud" and if ".ud" is removed, that should be the type of the
 *      table.
 * @param ref Assigned to the location in the registry.
 *
 * [-0, +1, v]
 */
void* evlua_obj_init(lua_State *L, size_t size, char* type_userdata, int* ref);

/**
 * Call when an object is destroyed.
 */
void evlua_obj_delete(lua_State* L);

/**
 * Retrieve the userdata for the table which contains a userdata in
 * LUA_NOREF.
 *
 * @param L The lua_State
 * @param ud The location of the userdata in the stack.
 * @param type_userdata The name of the metatable that the userdata is
 *      expected to be.
 *
 * [-0, +0, v]
 */
void* evlua_obj_check(lua_State *L, int ud, char* type_userdata);

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
