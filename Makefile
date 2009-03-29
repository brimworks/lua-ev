######################################################################
# If you want to make customizations to the build process, please see
# config.mk
######################################################################


# Default target:
all:

.PHONY: all clean test
.DELETE_ON_ERROR:

test: | all

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
include $(SRC)/evlua.mk

######################################################################
# Include makefiles for building lua and libev:
ifneq ($(BUILD_LUA),)
  include $(SRC)/3p/lua.mk
endif
ifneq ($(BUILD_LIBEV),)
  include $(SRC)/3p/libev.mk
endif
######################################################################
