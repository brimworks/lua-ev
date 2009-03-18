######################################################################
# If you want to make customizations to the build process, please see
# config.mk
######################################################################


# Default target:
all:

.PHONY: all clean
.DELETE_ON_ERROR:

# Include our configurations:
include $(dir $(lastword $(MAKEFILE_LIST)))config.mk

######################################################################
# Simple cleanup since we keep everything in a separate build dir.
clean:
	rm -rf $(BUILD)

######################################################################
# Keep our *.o files outside of our source tree:
$(dir.TMP)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) -MD -MP -c $(CFLAGS) $(abspath $<) -o $(abspath $@)

_files:=$(shell find $(dir.TMP) -name '*.d')
ifneq ($(_files),)
  include $(_files)
endif

######################################################################
# Build evlua.so:
all: $(dir.LIB_LUA)/evlua.so
OBJS:=$(addprefix $(dir.TMP)/, \
  evlua.o evlua_io.o evlua_loop.o evlua_timer.o evlua_watcher.o \
)
$(dir.LIB_LUA)/evlua.so: OBJS:=$(OBJS)
$(dir.LIB_LUA)/evlua.so: $(OBJS)
	@mkdir -p $(dir $@)
	$(LINKER) -o $@ $(addprefix -L,$(LIBS)) -llua -lev $(OBJS)
ifneq ($(BUILD_LUA),)
  $(dir.LIB_LUA)/evlua.so: $(dir.LIB)/liblua.so
  $(OBJS): | lua.headers
endif
ifneq ($(BUILD_LIBEV),)
  $(dir.LIB_LUA)/evlua.so: $(dir.LIB)/libev.so
  $(OBJS): | libev.headers
endif
######################################################################

######################################################################
# Include makefiles for building lua and libev:
ifneq ($(BUILD_LUA),)
  include $(SRC)/3p/lua.mk
endif
ifneq ($(BUILD_LIBEV),)
  include $(SRC)/3p/libev.mk
endif
######################################################################
