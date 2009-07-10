_pwd := $(pwd)

include $(make-common.dir)/tool/cc.mk
include $(make-common.dir)/tool/lua.mk
include $(make-common.dir)/layout.mk

_lib  := $(lua.c.lib.dir)/ev.so
_objs := $(call cc.c.to.o,$(addprefix $(_pwd)/, \
    lua_ev.c \
    lua_ev_io.c \
    lua_ev_loop.c \
    lua_ev_timer.c \
    lua_ev_shared.c \
))

all: | $(_lib)
$(_lib): cc.libs += lua ev
$(_lib): cc.objs := $(_objs)
$(_lib): $(_objs)
	$(cc.so.rule)

# How to run lua_zlib tests:
.PHONY: lua_ev.test
test: | lua_ev.test

lua ?= lua
lua_ev.test: | $(lua.c.lib.dir)/ev.so
lua_ev.test: lua.path += $(_pwd)/test
lua_ev.test: $(wildcard $(_pwd)/test/test_*.lua)
	@mkdir -p $(tmp.dir)
	cd $(tmp.dir); for t in $(filter-out %_help.lua,$^); do \
		echo "TESTING: $$t"; \
		env -i $(lua.run) $(lua) $$t; \
	done
