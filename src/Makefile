CC=gcc
AR=ar
TYPE=static
TARGET=libmoolticute.a
CFLAGS=-Wall

CFLAGS+=$(shell pkg-config --cflags json-c)
CFLAGS+=$(shell pkg-config --cflags libwebsockets)

LDFLAGS= -lpthread
LDFLAGS += $(shell pkg-config --libs json-c)
LDFLAGS += $(shell pkg-config --libs libwebsockets)

OBJECTS=moolticute_init.o moolticute_register_cb.o moolticute_cb_not_found.o moolticute_callback.o
OBJECTS+=moolticute_connect.o moolticute_cb_connected.o moolticute_cb_status_changed.o moolticute_cb_param_changed.o
OBJECTS+=moolticute_cb_version_changed.o moolticute_cb_card_db_metadata.o moolticute_cb_memorymgmt_changed.o
OBJECTS+=cmds/moolticute_cmd_query_random_number.o cmds/moolticute_cmd_request_device_uid.o cmds/moolticute_cmd_start_memory_management.o 
OBJECTS+=mooltipass_memory.o moolticute_cb_memorymgmt_data.o


.PHONY: clean

all: $(TARGET)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<


libmoolticute.a: $(OBJECTS)
	$(AR) -cvq $@ $(OBJECTS)

clean:
	-@rm -rf *.o *.a
