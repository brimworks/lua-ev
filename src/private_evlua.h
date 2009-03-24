#ifndef _PRIVATE_EVLUA_H
#define _PRIVATE_EVLUA_H

#include <lua.h>
#include <ev.h>

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
 * loop: lua table with:
 *    LUA_NOREF - the ev_loop C struct.
 *    1..N - active watchers.
 *
 * watcher: lua table with:
 *    1 - the ev_watcher_* C struct.
 *    2 - the callback function.
 *
 */

/**
 * Location in LUA_REGISTRYINDEX of evlua registry.
 */
extern int evlua_registry;


/**
 * Verify watchers are properly GC'ed:
 *
 * [-0, +1, -]
 */
int evlua_watcher_count(lua_State *L);

/**
 * Initialize various submodules.
 *
 * [-0, +1, e]
 */
int luaopen_evlua_timer(lua_State *L);
int luaopen_evlua_io(lua_State *L);
int luaopen_evlua_loop(lua_State *L);


/**
 * Extract the ev_loop* user data from the table in narg location on
 * the stack.
 *
 * [-0, +0, v]
 */
struct evlua_loop* evlua_check_loop(lua_State *L, int narg);

/**
 * Extract the ev_watcher* user data from the table in narg location
 * on the stack.  Validates that the watcher of specified watcher
 * type.
 *
 * [-0, +0, v]
 */
void* evlua_check_watcher(lua_State *L, int narg, char* type);

/**
 * Each watcher table is expected to contain these data elements.
 */
enum evlua_idx_t {
    idx_watcher=1, /** Watcher userdata */
    idx_func       /** The function called when an event occurs */
    /* if you add more items, be sure to update idx_max */
};
#define idx_max idx_func


/**
 *
 * [-0, +0, v]
 */
void evlua_unref_impl(lua_State *L, EV_P_ void** data);

void evlua_watcher_cb(lua_State *L, EV_P_ void* data, int revents);

/**
 * loop - The location on the stack of the loop table.
 *
 * func - The location on the stack of the callback function.
 *
 * size - The size of the watcher structures being created.
 *
 * type - The name of the metatable associated with the watcher table.
 *
 * ud_type - The name of the metatable associated with the user data.
 */
void* evlua_watcher_new(lua_State *L, int loop, int func, size_t size, char* type, char* ud_type);

int evlua_watcher_delete(lua_State *L);

struct ev_loop* evlua_watcher_loop(lua_State *L, int tbl);

// defined in evlua_loop.c:
lua_State* evlua_loop_state();
void evlua_loop_set_state(lua_State* L);

// We use the most-significant-bit (MSB) to determine the status of
// the ref_flag.  Here are some macros to make it easier to get/set
// the "self" and "ref_flag":
#define MSB_MASK  (1 << (sizeof(int)*8 - 1))
#define SIGN_MASK (1 << (sizeof(int)*8 - 2))

// Restore the sign extension;
#define get_self(data) \
    ( SIGN_MASK & ((int)data) ? MSB_MASK | ((int)data) : ~MSB_MASK & ((int)data) )

#define get_unref_flag(data) \
    ( MSB_MASK & ((int)data) )

#define merge_self_unref_flag(self, unref_flag) \
    (void*)( (unref_flag) ? MSB_MASK | (self) : ~MSB_MASK & (self) )

// Convenience macros:
#define evlua_maybe_unref(L, loop, watcher) do {                \
        if ( ! ev_is_pending(watcher) &&                        \
             ! ev_is_active(watcher) )                          \
            evlua_unref_impl((L), (loop), &(watcher)->data);    \
    } while(0)

#define evlua_ref(L, loop, watcher, idx, unref)                 \
    evlua_ref_impl((L), loop, &((watcher)->data), (idx), (unref))

// Type specific implementation in macro form:
#define IMPLEMENT_WATCHER_TYPE(TYPE)                                    \
    void evlua_##TYPE##_cb(struct ev_loop* loop, struct ev_##TYPE *w, int revents) { \
        lua_State*        L = evlua_loop_state();                       \
        evlua_watcher_cb(L, loop, w->data, revents);                    \
        evlua_maybe_unref(L, loop, w);                                  \
    }

#endif /* PRIVATE_EVLUA */
