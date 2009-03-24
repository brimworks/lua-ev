#include "private_evlua.h"
#include <lua.h>
#include <lauxlib.h>

static int live_watchers = 0;

// TODO:
int evlua_watcher_count(lua_State *L) {
    lua_pushinteger(L, live_watchers);
    return 1;
}

