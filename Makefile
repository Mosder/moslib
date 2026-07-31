CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -I./include -O2 -s
CFLAGS_TEST=-std=c99 -Wall -Wextra -lmoslib -Og -g
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

compile: memory structures
	$(CC) $(CFLAGS) -c src/tester.c

memory:
	$(CC) $(CFLAGS) -c src/mem/alloc.c

structures:
	$(CC) $(CFLAGS) -c src/ds/array.c

test:
	$(CC) $(CFLAGS_TEST) tests/main.c -o run_tests
	./run_tests
	rm run_tests

.PHONY: clean
clean:
	rm *.o
