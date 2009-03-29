######################################################################
# Build liblua.so:
all: $(dir.LIB)/liblua.so $(dir.BIN)/lua $(dir.BIN)/luac

OBJS:=$(addprefix $(dir.TMP)/3p/lua/src/, \
	lapi.o lcode.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o \
	lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o  \
	lundump.o lvm.o lzio.o \
	lauxlib.o lbaselib.o ldblib.o liolib.o lmathlib.o loslib.o ltablib.o \
	lstrlib.o loadlib.o linit.o \
)

$(dir.LIB)/liblua.so: OBJS:=$(OBJS)
$(dir.LIB)/liblua.so: $(OBJS)
	@mkdir -p $(dir $@)
	$(LIB_LINKER) -o $@ $(LINK_FLAGS.liblua) $(addprefix -L,$(LIBS)) $(OBJS)

######################################################################

OBJS:=$(addprefix $(dir.TMP)/3p/lua/src/, \
	luac.o print.o \
)

$(dir.BIN)/luac: OBJS:=$(OBJS)
$(dir.BIN)/luac: $(OBJS) $(dir.LIB)/liblua.so
	@mkdir -p $(dir $@)
	$(APP_LINKER) -o $@ $(LINK_FLAGS.luac) -llua $(addprefix -L,$(LIBS)) $(OBJS)

######################################################################

OBJS:=$(addprefix $(dir.TMP)/3p/lua/src/, \
	lua.o \
)

$(dir.BIN)/lua: OBJS:=$(OBJS)
$(dir.BIN)/lua: $(OBJS) $(dir.LIB)/liblua.so
	@mkdir -p $(dir $@)
	$(APP_LINKER) -o $@ $(LINK_FLAGS.lua) -llua $(addprefix -L,$(LIBS)) $(OBJS)

######################################################################
# Export headers:
$(dir.INCLUDE)/%.h: $(SRC)/3p/lua/src/%.h
	@mkdir -p $(dir $@)
	cp -f $< $@
.PHONY: lua.headers
all lua.headers: $(addprefix $(dir.INCLUDE)/,lua.h luaconf.h lualib.h lauxlib.h)
