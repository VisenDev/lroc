CLANG_FLAGS=-fsanitize=address -fsanitize=undefined
CFLAGS=-Wall -Wextra -Werror -Wpedantic -std=c99 -g $(CLANG_FLAGS)
EXE=./.build/main
CC=clang

ncurses: main.c
	mkdir -p .build
	$(CC) $(CFLAGS) main.c -o $(EXE) -lncurses -DBACKEND_NCURSES

ansi: main.c 
	mkdir -p .build
	$(CC) $(CFLAGS) main.c -o $(EXE) -DBACKEND_ANSI

debug: main.c  
	mkdir -p .build
	$(CC) $(CFLAGS) main.c -o $(EXE) -DBACKEND_DUMMY
	lldb $(EXE)

run: ncurses
	$(EXE)

