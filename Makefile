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

CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -fPIC -I./include -I./compat -O2 -c
SRCS:=$(shell find src compat | grep "\.c$$")
CLEANUP=$(MAKE) clean --no-print-directory

TESTS_CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -I$(INC_DIR) -I./compat -Og -ggdb -c
TESTS_LDFLAGS=-L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR) -lmoslib
TESTS_SRCS:=$(shell find tests compat | grep "\.c$$")

define probe_fun
	$(shell printf '#include "compat.h"\n#include <$(1)>\nint main(void){$(2);return 0;}' \
		| $(CC) $(CFLAGS) -x c - -o /dev/null 2>/dev/null \
		|| echo -DNO_$(shell echo $(2) | tr a-z A-Z))
endef

PROBED:= \
	$(call probe_fun,string.h,memmem) \
	$(call probe_fun,string.h,strdup)

CFLAGS+=$(PROBED)
TESTS_CFLAGS+=$(PROBED)

both: CLEANUP=true
both: clean shared static
	@$(MAKE) clean --no-print-directory

shared: clean compile headers
	$(LIB_SUDO) $(CC) -shared *.o -o $(LIB_DIR)/libmoslib.so
	$(LIB_SUDO) strip -x $(LIB_DIR)/libmoslib.so
	@$(CLEANUP)

static: clean compile headers
	$(LIB_SUDO) $(AR) $(LIB_DIR)/libmoslib.a *.o
	$(LIB_SUDO) strip -S $(LIB_DIR)/libmoslib.a
	@$(CLEANUP)

headers:
	$(INC_SUDO) cp -r include/* $(INC_DIR)

compile:
	$(CC) $(CFLAGS) $(SRCS)

test: clean
	$(CC) $(TESTS_CFLAGS) $(TESTS_SRCS)
	$(CC) $(TESTS_LDFLAGS) *.o -o run_tests
	./run_tests
	@$(CLEANUP)

clean:
	rm *.o run_tests 2>/dev/null || true
	rm -rf *.dSYM/ 2>/dev/null || true
