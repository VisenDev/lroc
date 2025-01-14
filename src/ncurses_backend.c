#include "backend.h"
#include <ncurses.h>
#include <stdlib.h>
#include "pimbs/src/allocator.h"

typedef struct {
    WINDOW* front;
    WINDOW* back;
    Allocator a;
} NCursesContext;

InputEvent ncurses_input(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    const char ch = wgetch(ctx->front);
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
    NCursesContext * ctx = self.ctx;
    wmove(ctx->back, cmd.y, cmd.x);
    wattron(ctx->back, COLOR_PAIR(cmd.color));
    waddch(ctx->back, cmd.ch);
    wattroff(ctx->back, COLOR_PAIR(cmd.color));
}

void ncurses_clear_screen(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    werase(ctx->back);
}

void ncurses_begin_rendering(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    /*clean the buffer before rendering*/
    werase(ctx->back);
}

void ncurses_finish_rendering(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    WINDOW* temp = NULL;

    /* Swap the front and back buffers*/
    wrefresh(ctx->back);
    temp = ctx->front;
    ctx->front = ctx->back;
    ctx->back = temp;
}

void ncurses_deinit(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    delwin(ctx->front);
    delwin(ctx->back);
    endwin();  /* End curses mode.*/
    ctx->a.free(ctx->a, ctx);
}

Backend ncurses_backend(Allocator a) {
    Backend self = {0};
    NCursesContext * ctx = a.alloc(a, sizeof(NCursesContext));
    if(ctx == NULL) {
        fprintf(stderr, "Memory allocation failure");
        abort();
    }

    self.ctx = ctx;
    self.input = ncurses_input;
    self.render = ncurses_render;
    self.clear_screen = ncurses_clear_screen;
    self.deinit = ncurses_deinit;
    self.finish_rendering = ncurses_finish_rendering;
    self.begin_rendering = ncurses_begin_rendering;

    initscr();           /* Start curses mode.              */
    raw();               /* Send input on every keystroke.  */
    noecho();            /* Don't echo input.               */
    curs_set(0);         /* Disable cursor.                 */

    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Your terminal does not support colors.\n");
        abort();
    }

    if (start_color() != OK) {
        endwin();
        fprintf(stderr, "Failed to initialize color mode.\n");
        abort();
    }

    if (COLORS < 256) {
        endwin();
        fprintf(stderr, "Your terminal does not support 256 colors.\n");
        abort();
    }

    {
        short i = 0;
        for (i = 0; i < 256; i++) {
            /* Foreground: color i, Background: black.*/
            init_pair(i + 1, i, COLOR_BLACK);
        }
    }

    /* Initialize double ctx-> */
    ctx->front = newwin(LINES, COLS, 0, 0);
    ctx->back = newwin(LINES, COLS, 0, 0);
    ctx->a = a;

    if (!ctx->front || !ctx->back) {
        endwin();
        fprintf(stderr, "Failed to create ncurses windows.\n");
        abort();
    }

    return self;
}
