BUILD := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))build)
SRC   := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))src)

# By default we are configured to compile lua, libev, and evlua all
# from the supplied source here.  Customize this makefile to suite
# your needs.
#
# Build directory layout:
dir.LIB     := $(BUILD)/lib
dir.INCLUDE := $(BUILD)/include
dir.BIN     := $(BUILD)/bin
dir.LIB_LUA := $(BUILD)/lib/lua/5.1
dir.TMP     := $(BUILD)/private

# Set to empty value if you don't want these built:
BUILD_LUA   := 1
BUILD_LIBEV := 1

# Path to any include directories needed:
INCLUDES = $(dir.INCLUDE)

# Path to any lib directories neede:
LIBS = $(dir.LIB)

# Platform specific compiler flags:
CFLAGS = -Wall -Werror -fPIC -std=gnu99 -pedantic -g $(addprefix -I,$(INCLUDES))
LINKER = gcc -dynamiclib

