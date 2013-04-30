
find_path(LUA_INCLUDE_DIR lua.h
	HINTS
	$ENV{LUA_DIR}
	PATH_SUFFIXES include/lua52 include/lua5.2 include/lua include
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
	NAMES lua52 lua5.2 lua-5.2 lua
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

find_package_handle_standard_args(Lua52 DEFAULT_MSG LUA_LIBRARIES LUA_INCLUDE_DIR)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES LUA_LIBRARY LUA_MATH_LIBRARY)
