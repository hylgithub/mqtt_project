
CROSS_COMPILE=

ifeq ($(V), 1)
 Q = 
else
 Q = @
endif

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
RANLIB = $(CROSS_COMPILE)ranlib
STRIP = $(CROSS_COMPILE)strip

CFLAGS += -I ./../service
LDFLAGS += -Wl,--start-group -lc -lgcc -Wl,--end-group
LDFLAGS += -L ./
#LDFLAGS += -Wl,-Bstatic -lcrypto -Wl

LIBNAME = libmqtt.a 

LIBOBJS = mqtt.o handle.o mqtt_send.o


echo_cmd = $(if $(Q), $(Q)echo $(1))
create_cmd = $(call echo_cmd, 'Create	$@')

.PHONY: all lib
all: $(LIBNAME)
	$(call echo_cmd,'Compile success')


$(LIBNAME): $(LIBOBJS)
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@
	$(call create_cmd)

%.o: %.c
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<
	$(call echo_cmd, 'CC	$@')

%.i: %.c
	$(Q)$(CC) $(CFLAGS) -E -o $@ $<
	$(call echo_cmd, 'CC	-E $@')

%.S: %.c
	$(Q)$(CC) $(CFLAGS) -S -o $@ $<
	$(call echo_cmd, 'CC	-S $@')

tags: *.c *.h
	$(Q)ctags $^

.PHONY: clean cleanlib cleanall
clean:
	$(Q)rm -fr *.i *.o *.S *.s tags
	$(Q)rm -fr $(APPNAME) $(APPNAME).gdb
	$(call echo_cmd, 'Clean objects')

cleanlib:
	$(Q)rm -fr $(LIBNAME)
	$(call echo_cmd, 'Clean library objects')

cleanall: clean cleanlib
	$(call echo_cmd, 'Clean all objects')

