
find_path(LUA_INCLUDE_DIR lua.h
	HINTS
	$ENV{LUA_DIR}
	PATH_SUFFIXES include include/lua include/lua53 include/lua5.3 include/lua52 include/lua5.2 include/lua51 include/lua5.1
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt
)

find_library(LUA_LIBRARY
	NAMES lua lua53 lua5.3 lua52 lua5.2 lua-5.2 lua51 lua5.1 luajit-5.1 luajit51 luajit5.1
	HINTS
	$ENV{LUA_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt
)

if(LUA_LIBRARY)
	if(UNIX AND NOT APPLE)
		find_library(LUA_MATH_LIBRARY m)
		set( LUA_LIBRARIES "${LUA_LIBRARY};${LUA_MATH_LIBRARY}" CACHE STRING "Lua Libraries")
	else(UNIX AND NOT APPLE)
		set( LUA_LIBRARIES "${LUA_LIBRARY}" CACHE STRING "Lua Libraries")
	endif(UNIX AND NOT APPLE)
endif(LUA_LIBRARY)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Lua5X DEFAULT_MSG LUA_LIBRARIES LUA_INCLUDE_DIR)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES LUA_LIBRARY LUA_MATH_LIBRARY)
