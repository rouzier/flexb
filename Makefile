
CFLAGS=-Iinclude -O2 -Wall

tests: tests/test
	tests/test

tests/test: tests/test.o

tests/test.o: include/snf/flexb.h tests/test.c
