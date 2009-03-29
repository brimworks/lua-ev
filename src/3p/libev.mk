######################################################################
# Build libev.so:
all: $(dir.LIB)/libev.so
OBJS:=$(addprefix $(dir.TMP)/3p/libev/, \
        ev.o event.o \
)

#ev_epoll.o ev_kqueue.o ev_poll.o \
#        ev_port.o ev_select.o ev_win32.o event.o \

$(dir.LIB)/libev.so: OBJS:=$(OBJS)
$(dir.LIB)/libev.so: $(OBJS)
	@mkdir -p $(dir $@)
	$(LIB_LINKER) -o $@ $(addprefix -L,$(LIBS)) $(OBJS)

######################################################################
# Export headers:
$(dir.INCLUDE)/%.h: $(SRC)/3p/libev/%.h
	@mkdir -p $(dir $@)
	cp -f $< $@
.PHONY .INTERMEDIATE: libev.headers
all libev.headers: $(addprefix $(dir.INCLUDE)/,ev.h ev++.h event.h)
