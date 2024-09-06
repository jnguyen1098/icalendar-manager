CC = clang-18
CFLAGS = -std=c17 -gdwarf-4 -ferror-limit=99999\
		 -Wall -Wpedantic -Weverything -Wextra\
		 -Wno-documentation -Wno-reserved-macro-identifier -Wno-newline-eof\
		 -Wno-declaration-after-statement -Wno-padded -Wno-sign-conversion\
		 -Wno-shorten-64-to-32 -Wno-unsafe-buffer-usage -Wno-shadow\
		 -Wno-missing-prototypes -Wno-sign-compare -Wno-unused-parameter\
		 -Wno-cast-qual -Wno-sometimes-uninitialized -Wno-vla -Wno-switch-default\
		 -Wno-unreachable-code-return -Wno-documentation-unknown-command\
		 -Wno-unused-macros -Wno-implicit-float-conversion -Wno-double-promotion\
		 -Wno-tautological-pointer-compare -Wno-misleading-indentation\
		 -Wno-covered-switch-default -Wno-parentheses-equality
CLINK = -Lbin -lfinal

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
	LIB := libfinal.dylib
else ifeq ($(UNAME_S), Linux)
	LIB := libfinal.so
endif

# all
# run

all: LinkedListAPI.o CalendarParser.o CalendarHelper.o
	$(CC) $(CFLAGS) -shared -o bin/$(LIB) bin/LinkedListAPI.o bin/CalendarParser.o bin/CalendarHelper.o

LinkedListAPI.o:
	$(CC) $(CFLAGS) -c -fPIC src/LinkedListAPI.c -Iinclude -o bin/LinkedListAPI.o

CalendarParser.o:
	$(CC) $(CFLAGS) -c -fPIC src/CalendarParser.c -Iinclude  -o bin/CalendarParser.o

CalendarHelper.o:
	$(CC) $(CFLAGS) -c -fPIC src/CalendarHelper.c -Iinclude -o bin/CalendarHelper.o

clean:
	rm -rf bin/*.o bin/*.so
	rm -rf bin/A1m1 bin/A2m1 bin/A2m2 bin/A2m2v bin/CalendarDemo
	rm -rf bin/TEST/WRITECALEXPORTS/

CalendarDemo: CalendarDemo.o all
	$(CC) $(CFLAGS) $(CLINK) -o bin/CalendarDemo bin/CalendarDemo.o -Iinclude

CalendarDemo.o: src/CalendarDemo.c include/CalendarHelper.h
	$(CC) $(CFLAGS) -c -fPIC src/CalendarDemo.c -o bin/CalendarDemo.o -Iinclude

run: CalendarDemo
	LD_LIBRARY_PATH=bin ./bin/CalendarDemo

test: make_tests run1 run2 run3 run4 run1v run2v run3v run4v
make_tests: all compileHarness1 compileHarness2 compileHarness3 compileHarness4

# Compile Test Harness 1 (A1 createCalendar error code assertions)
compileHarness1: src/A1harness.c
	$(CC) $(CFLAGS) -c -fPIC src/A1harness.c -o bin/A1m1.o -Iinclude
	$(CC) $(CFLAGS) $(CLINK) -o bin/A1m1 bin/A1m1.o -Iinclude

# Compile Test Harness 2 (A2 writeCalendar calendar writing assertions)
compileHarness2: src/A2m1harness.c
	$(CC) $(CFLAGS) -c -fPIC src/A2m1harness.c -o bin/A2m1.o -Iinclude
	$(CC) $(CFLAGS) $(CLINK) -o bin/A2m1 bin/A2m1.o -Iinclude

# Compile Test Harness 3 (A2 validateCalendar error code assertions)
compileHarness3: src/A2m2harness.c
	$(CC) $(CFLAGS) -c -fPIC src/A2m2harness.c -o bin/A2m2.o -Iinclude
	$(CC) $(CFLAGS) $(CLINK) -o bin/A2m2 bin/A2m2.o -Iinclude

# Compile Test Harness 4 (A2 validateCalendar precedence vulnerability check)
compileHarness4: src/A2m2harnessv.c
	$(CC) $(CFLAGS) -c -fPIC src/A2m2harnessv.c -o bin/A2m2v.o -Iinclude
	$(CC) $(CFLAGS) $(CLINK) -o bin/A2m2v bin/A2m2v.o -Iinclude

###############################################################################

# Run Test Harness 1
run1: compileHarness1
	cd bin && LD_LIBRARY_PATH=. ./A1m1 && cd ..

# Run Test Harness 2
run2: compileHarness2
	cd bin && LD_LIBRARY_PATH=. ./A2m1 && cd ..
	rm -rf bin/TEST/WRITECALEXPORTS/

# Run Test Harness 3
run3: compileHarness3
	cd bin && LD_LIBRARY_PATH=. ./A2m2 && cd ..

# Run Test Harness 4
run4: compileHarness4
	cd bin && LD_LIBRARY_PATH=. ./A2m2v && cd ..

###############################################################################

# Run Test Harness 1 (Valgrind)
run1v: compileHarness1
	cd bin && LD_LIBRARY_PATH=. valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./A1m1 && cd ..

# Run Test Harness 2 (Valgrind)
run2v: compileHarness2
	rm -rf bin/TEST/WRITECALEXPORTS/
	cd bin && LD_LIBRARY_PATH=. valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./A2m1 && cd ..

# Run Test Harness 3 (Valgrind)
run3v: compileHarness3
	cd bin && LD_LIBRARY_PATH=. valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./A2m2 && cd ..

# Run Test Harness 4 (Valgrind)
run4v: compileHarness4
	cd bin && LD_LIBRARY_PATH=. valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./A2m2v && cd ..

###############################################################################


