#include "private_evlua.h"
#include <lua.h>
#include <lauxlib.h>

int io_new(lua_State *L) {
    return 0;
}

void evlua_open_io(lua_State *L) {
    // At this point only one static method:
    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, io_new);
    lua_setfield(L, -2, "new");
}
