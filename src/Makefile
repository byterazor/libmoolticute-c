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

CBS=callbacks/moolticute_cb_not_found.o
CBS+=callbacks/moolticute_cb_connected.o callbacks/moolticute_cb_status_changed.o callbacks/moolticute_cb_param_changed.o
CBS+=callbacks/moolticute_cb_version_changed.o callbacks/moolticute_cb_card_db_metadata.o callbacks/moolticute_cb_memorymgmt_changed.o
CBS+=callbacks/moolticute_cb_failed_memorymgmt.o callbacks/moolticute_cb_application_id.o callbacks/moolticute_cb_progress.o
CBS+=callbacks/moolticute_cb_memorymgmt_data.o

OBJECTS=moolticute_init.o  moolticute_callback.o
OBJECTS+=moolticute_connect.o moolticute_register_cb.o
OBJECTS+=cmds/moolticute_cmd_query_random_number.o cmds/moolticute_cmd_request_device_uid.o cmds/moolticute_cmd_start_memory_management.o
OBJECTS+=mooltipass_memory.o  cmds/moolticute_cmd_stop_memory_management.o
OBJECTS+=cmds/moolticute_cmd_get_application_id.o
OBJECTS+=cmds/moolticute_cmd_request_password.o
OBJECTS+=moolticute_array.o
.PHONY: clean

all: $(TARGET)

%.o: %.c moolticute.h
	$(CC) -c -o $@ $(CFLAGS) $<


libmoolticute.a: $(OBJECTS) $(CBS)
	$(AR) -cvq $@ $(OBJECTS)

clean:
	-@rm -rf *.o *.a
	-@rm -rf cmds/*.o
