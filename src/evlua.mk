
all: $(dir.LIB_LUA)/evlua.so
test: | evlua.test

TESTS := $(wildcard $(SRC)/test_evlua_*)

LUA=lua $1
evlua.test: TESTS:=$(TESTS)
evlua.test: $(dir.LIB_LUA)/evlua.so $(TESTS)
	cd $(SRC); for f in $(TESTS); do \
		echo "TESTING: $$f"; \
		echo "run $$f" > $(dir.TMP)/backtrace.gdb; \
		echo "where"  >> $(dir.TMP)/backtrace.gdb; \
		env -i LUA_CPATH='$(dir.LIB_LUA)/?.so' DYLD_LIBRARY_PATH=$(subst $(SPACE),:,$(LIBS)) PATH=$(BINS):$$PATH $(call LUA,"$$f"); \
	done
.PHONY: evlua.test

backtrace: evlua.test
backtrace: LUA=gdb -f -x $(dir.TMP)/backtrace.gdb -batch lua

.PHONY: backtrace

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
