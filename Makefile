CC=cc
CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -fPIC -I./include -O2
CFLAGS_TEST=-std=c99 -Wall -Wextra -Wpedantic -Og -ggdb
AR=ar rcs
RUN_TESTS=./run_tests

ifeq ($(shell uname -s),Darwin)
	RUN_TESTS=DYLD_FALLBACK_LIBRARY_PATH="/usr/local/lib:$DYLD_FALLBACK_LIBRARY_PATH"
	RUN_TESTS+= ./run_tests; rm -rf run_tests.dSYM/
endif

both: shared static clean

shared: compile headers
	$(CC) -shared *.o -o libmoslib.so
	strip -x libmoslib.so
	sudo mv libmoslib.so /usr/local/lib

static: compile headers
	$(AR) libmoslib.a *.o
	strip -S libmoslib.a
	sudo mv libmoslib.a /usr/local/lib

headers:
	sudo cp -r include/* /usr/local/include

compile: structures
	$(CC) $(CFLAGS) -c src/tester.c
	$(CC) $(CFLAGS) -c src/safe.c
	$(CC) $(CFLAGS) -c src/string.c

structures:
	$(CC) $(CFLAGS) -c src/ds/array.c
	$(CC) $(CFLAGS) -c src/ds/hashmap.c

test:
	$(CC) $(CFLAGS_TEST) -c tests/safe_tests.c
	$(CC) $(CFLAGS_TEST) -c tests/ds/array_tests.c
	$(CC) $(CFLAGS_TEST) -c tests/string_tests.c
	$(CC) $(CFLAGS_TEST) -c tests/ds/hashmap_tests.c
	$(CC) $(CFLAGS_TEST) -lmoslib *.o tests/tests.c -o run_tests
	$(RUN_TESTS)
	rm run_tests *.o

.PHONY: clean
clean:
	rm *.o
