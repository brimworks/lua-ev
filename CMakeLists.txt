# This cmake file was inspired by:
# Copyright (C) 2007-2009 LuaDist.
# Submitted by David Manura
# Redistribution and use of this file is allowed according to the
# terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own
# license.

PROJECT(lua-ev C)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# Basic configurations
  SET(INSTALL_CMOD share/lua/cmod CACHE PATH "Directory to install Lua binary modules (configure lua via LUA_CPATH)")
# / configs

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find libev
  FIND_LIBRARY (LIBEV_LIBRARY NAMES ev)
  FIND_PATH (LIBEV_INCLUDE_DIR ev.h
    PATH_SUFFIXES include/ev include
    ) # Find header
  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(libev  DEFAULT_MSG  LIBEV_LIBRARY LIBEV_INCLUDE_DIR)
# / Find libarchive

# Find lua
  FIND_PACKAGE(Lua5X REQUIRED)
# / Find lua

# Define how to build ev.so:
  INCLUDE_DIRECTORIES(${LIBEV_INCLUDE_DIR} ${LUA_INCLUDE_DIR})
  ADD_LIBRARY(cmod_ev MODULE
    lua_ev.c
    )
  SET_TARGET_PROPERTIES(cmod_ev PROPERTIES PREFIX "")
  SET_TARGET_PROPERTIES(cmod_ev PROPERTIES OUTPUT_NAME ev)
  TARGET_LINK_LIBRARIES(cmod_ev ${LUA_LIBRARIES} ${LIBEV_LIBRARY})
# / build ev.so

# Define how to test ev.so:
  INCLUDE(CTest)
  FIND_PROGRAM(LUA NAMES lua lua.bat)
  ADD_TEST(ev_io ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ev_io.lua ${CMAKE_CURRENT_SOURCE_DIR}/test/ ${CMAKE_CURRENT_BINARY_DIR}/)
  ADD_TEST(ev_loop ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ev_loop.lua ${CMAKE_CURRENT_SOURCE_DIR}/test/ ${CMAKE_CURRENT_BINARY_DIR}/)
  ADD_TEST(ev_timer ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ev_timer.lua ${CMAKE_CURRENT_SOURCE_DIR}/test/ ${CMAKE_CURRENT_BINARY_DIR}/)
  ADD_TEST(ev_idle ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ev_idle.lua ${CMAKE_CURRENT_SOURCE_DIR}/test/ ${CMAKE_CURRENT_BINARY_DIR}/)
  ADD_TEST(ev_signal ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ev_signal.lua ${CMAKE_CURRENT_SOURCE_DIR}/test/ ${CMAKE_CURRENT_BINARY_DIR}/)
  ADD_TEST(ev_child ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ev_child.lua ${CMAKE_CURRENT_SOURCE_DIR}/test/ ${CMAKE_CURRENT_BINARY_DIR}/)
  ADD_TEST(ev_stat ${LUA} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ev_stat.lua ${CMAKE_CURRENT_SOURCE_DIR}/test/ ${CMAKE_CURRENT_BINARY_DIR}/)
  SET_TESTS_PROPERTIES(ev_io ev_loop ev_timer ev_signal ev_idle ev_child ev_stat
                       PROPERTIES
                       FAIL_REGULAR_EXPRESSION
                       "not ok")

# / test ev.so

# Where to install stuff
  INSTALL (TARGETS cmod_ev DESTINATION ${INSTALL_CMOD})
# / Where to install.
