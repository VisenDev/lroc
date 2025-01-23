CFLAGS=-fsanitize=address -fsanitize=undefined -Wall -Wextra -Wpedantic -std=c99 -g
EXE=./.build/main

ncurses: main.c
	mkdir -p .build
	cc $(CFLAGS) main.c -o $(EXE) -lncurses -DBACKEND_NCURSES

ansi: main.c 
	mkdir -p .build
	cc $(CFLAGS) main.c -o $(EXE) -DBACKEND_ANSI

debug: main.c  
	mkdir -p .build
	cc $(CFLAGS) main.c -o $(EXE) -DBACKEND_DUMMY
	lldb $(EXE)


run: ncurses
	$(EXE)

