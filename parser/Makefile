CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -g
CLINK = -L../ -lfinal

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
	LIB := libfinal.dylib
else ifeq ($(UNAME_S), Linux)
	LIB := libfinal.so
endif

all: LinkedListAPI.o CalendarParser.o CalendarHelper.o
	$(CC) -shared -o ../$(LIB) bin/LinkedListAPI.o bin/CalendarParser.o bin/CalendarHelper.o

LinkedListAPI.o:
	$(CC) $(CFLAGS) -c -fPIC src/LinkedListAPI.c -Iinclude -o bin/LinkedListAPI.o

CalendarParser.o:
	$(CC) $(CFLAGS) -c -fPIC src/CalendarParser.c -Iinclude  -o bin/CalendarParser.o

CalendarHelper.o:
	$(CC) $(CFLAGS) -c -fPIC src/CalendarHelper.c -Iinclude -o bin/CalendarHelper.o

clean:
	rm -rf bin/*.o bin/*.so ../$(LIB)
	rm -rf $(LIB)

CalendarDemo: CalendarDemo.o all
	$(CC) $(CFLAGS) $(CLINK) -o bin/CalendarDemo bin/CalendarDemo.o -Iinclude

CalendarDemo.o: src/CalendarDemo.c include/CalendarHelper.h
	$(CC) $(CFLAGS) -c -fPIC src/CalendarDemo.c -o bin/CalendarDemo.o -Iinclude

run: CalendarDemo
	LD_LIBRARY_PATH=../ ./bin/CalendarDemo
