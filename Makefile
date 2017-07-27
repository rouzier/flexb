
CFLAGS=-Iinclude -O2 -Wall

.phony: tests

tests: tests/test
	tests/test

tests/test: tests/test.o

tests/test.o: include/flexb/flexb.h tests/test.c

.phony: clean

clean:
	rm -f tests/test.o tests/test
