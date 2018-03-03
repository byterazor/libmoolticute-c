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
OBJECTS+=moolticute_connect.o

.PHONY: clean

all: $(TARGET)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<


libmoolticute.a: $(OBJECTS)
	$(AR) -cvq $@ $(OBJECTS)

clean:
	-@rm -rf *.o *.a
