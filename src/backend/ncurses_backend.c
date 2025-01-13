#include "backend.h"
#include <ncurses.h>
#include <stdlib.h>


InputEvent ncurses_input(struct Backend self) {
    const char ch = getch();
    switch(ch) {
        case 'j': return EVENT_DOWN;
        case 'k': return EVENT_UP;
        case 'h': return EVENT_LEFT;
        case 'l': return EVENT_RIGHT;
        case '.': return EVENT_WAIT;
        case 'q': return EVENT_QUIT;
        default: return EVENT_NIL;
    }
}

void ncurses_render(struct Backend self, RenderEvent cmd) {
    (void)self;
    move(cmd.y, cmd.x);
    attron(COLOR_PAIR(cmd.color));
    printw("%c", cmd.ch);
    attroff(COLOR_PAIR(cmd.color));
}

void ncurses_clear_screen(struct Backend self) {
    (void) self;
    clear();
}

void ncurses_begin_rendering(struct Backend self) {
    (void)self;
}

void ncurses_finish_rendering(struct Backend self) {
    (void)self;
    refresh();
}

void ncurses_deinit(struct Backend self) {
    (void) self;
	endwin();			/* End curses mode		  */
}


Backend ncurses_backend() {
    Backend self = {0};
    self.input = ncurses_input;
    self.render = ncurses_render;
    self.clear_screen = ncurses_clear_screen;
    self.deinit = ncurses_deinit;
    self.finish_rendering = ncurses_finish_rendering;
    self.begin_rendering = ncurses_begin_rendering;


	initscr();			/* Start curses mode 		  */
    raw();
    noecho();

    if (!has_colors()) {
        endwin();
        printf("Your terminal does not support colors.\n");
        abort();
    }

    if (start_color() != OK) {
        endwin();
        printf("Failed to initialize color mode.\n");
        abort();
    }

    if (COLORS < 256) {
        endwin();
        printf("Your terminal does not support 256 colors.\n");
        abort();
    }

    // Initialize 256 color pairs
    for (int i = 0; i < 256; i++) {
        init_pair(i + 1, i, COLOR_BLACK); // Foreground: color i, Background: black
    }

    return self;
}
