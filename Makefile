CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Wpointer-arith -fPIC -I./include -O2 -s
CFLAGS_TEST=-std=c99 -Wall -Wextra -Wpointer-arith -Og -ggdb
AR=ar rcs

both: shared static clean

shared: compile headers
	$(CC) -shared *.o -o libmoslib.so
	sudo mv libmoslib.so /usr/local/lib

static: compile headers
	$(AR) libmoslib.a *.o
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
	./run_tests
	rm run_tests *.o

.PHONY: clean
clean:
	rm *.o
