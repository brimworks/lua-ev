
all: $(dir.LIB_LUA)/evlua.so
test: | $(dir.TMP)/evlua.test

TESTS := $(wildcard $(SRC)/test_evlua_*)

$(dir.TMP)/evlua.test: TESTS:=$(TESTS)
$(dir.TMP)/evlua.test: $(dir.LIB_LUA)/evlua.so $(TESTS)
	cd $(SRC); for f in $(TESTS); do \
		echo "TESTING: $$f"; \
		env -i LUA_CPATH='$(dir.LIB_LUA)/?.so' DYLD_LIBRARY_PATH=$(subst $(SPACE),:,$(LIBS)) PATH=$(BINS) lua "$$f"; \
	done && touch $@

#backtrace: $(SO)
#	DYLD_LIBRARY_PATH=$(EV_LIB):$(LUA_LIB) PATH=$(LUA_BIN):$$PATH gdb -f -x backtrace.gdb -batch lua


OBJS:=$(addprefix $(dir.TMP)/, \
  evlua.o evlua_io.o evlua_loop.o evlua_timer.o evlua_shared.o \
)

$(dir.LIB_LUA)/evlua.so: OBJS:=$(OBJS)
$(dir.LIB_LUA)/evlua.so: $(OBJS)
	@mkdir -p $(dir $@)
	$(LIB_LINKER) -o $@ $(addprefix -L,$(LIBS)) -llua -lev $(OBJS)

# Properly model our dependency:
ifneq ($(BUILD_LUA),)
  $(dir.LIB_LUA)/evlua.so: $(dir.LIB)/liblua.so
  $(OBJS): | lua.headers
endif
ifneq ($(BUILD_LIBEV),)
  $(dir.LIB_LUA)/evlua.so: $(dir.LIB)/libev.so
  $(OBJS): | libev.headers
endif
