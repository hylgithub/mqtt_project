#export PATH=/home/share/mosquitto/mqtt/socket:$(PATH)
CROSS_COMPILE=

ifeq ($(V), 1)
 Q = 
else
 Q = @
endif

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld

CFLAGS += -Wall

#CFLAGS += -I /home/share/mosquitto/mqtt/service/lib_service
#LDFLAGS += -L /home/share/mosquitto/mqtt/service/lib_service
CFLAGS += -I ./../lib
LDFLAGS += -L ./../lib
LDFLAGS += -Wl -lpthread -lmqtt -Wl

APPNAME = service

#OBJS = service.o handle.o mqtt_send.o
OBJS = service.o

echo_cmd = $(if $(Q), $(Q)echo $(1))
create_cmd = $(call echo_cmd, 'Create	$@')

all: $(APPNAME)
	$(call echo_cmd,'Compile success')

$(APPNAME): $(OBJS)
	$(Q)$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
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

.PHONY: clean
clean:
	$(Q)rm -fr *.i *.o *.S *.s $(APPNAME) $(APPNAME).gdb
	$(call echo_cmd, 'Clean all objects')
