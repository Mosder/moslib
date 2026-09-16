-include .make
ifeq ($(CC),)
	CC=cc
endif
ifeq ($(TARGET_PREFIX),)
	TARGET_PREFIX=/usr/local
endif

AR=ar rcs

LIB_DIR=$(TARGET_PREFIX)/lib
LIB_SUDO:=$(shell test -w $(LIB_DIR) || echo sudo)
INC_DIR=$(TARGET_PREFIX)/include
INC_SUDO:=$(shell test -w $(INC_DIR) || echo sudo)

CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -fPIC -I./include -O2 -c
SRCS:=$(shell find src | grep \\.c$)

TESTS_CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -I$(INC_DIR) -Og -ggdb -c
TESTS_LDFLAGS=-L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR) -lmoslib
TESTS_SRCS:=$(shell find tests | grep \\.c$)

both: clean shared static
	$(MAKE) clean

shared: clean compile headers
	$(LIB_SUDO) $(CC) -shared *.o -o $(LIB_DIR)/libmoslib.so
	$(LIB_SUDO) strip -x $(LIB_DIR)/libmoslib.so

static: clean compile headers
	$(LIB_SUDO) $(AR) $(LIB_DIR)/libmoslib.a *.o
	$(LIB_SUDO) strip -S $(LIB_DIR)/libmoslib.a

headers:
	$(INC_SUDO) cp -r include/* $(INC_DIR)

compile:
	$(CC) $(CFLAGS) $(SRCS)

test: clean
	$(CC) $(TESTS_CFLAGS) $(TESTS_SRCS)
	$(CC) $(TESTS_LDFLAGS) *.o -o run_tests
	./run_tests
	$(MAKE) clean

clean:
	rm *.o run_tests 2>/dev/null || true
	rm -rf *.dSYM/ 2>/dev/null || true
