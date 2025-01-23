CFLAGS=-fsanitize=address -fsanitize=undefined -Wall -Wextra -Wpedantic -std=c99
EXE=./.build/main

ncurses: main.c Makefile
	mkdir -p .build
	cc $(CFLAGS) main.c -o $(EXE) -lncurses -DBACKEND_NCURSES

ansi: main.c Makefile
	mkdir -p .build
	cc $(CFLAGS) main.c -o $(EXE) -DBACKEND_ANSI


run: $(EXE)
	$(EXE)

