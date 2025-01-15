#include "backend.h"
#include "map.h"
#include <ncurses.h>
#include <stdlib.h>
#include "pimbs/src/allocator.h"

typedef struct {
    WINDOW* front;
    WINDOW* back;
    Allocator a;
    uint8_t display_i;
} NCursesContext;

Action ncurses_input(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    const char ch = wgetch(ctx->front);
    switch(ch) {
        case 'j': return ACTION_DOWN;
        case 'k': return ACTION_UP;
        case 'h': return ACTION_LEFT;
        case 'l': return ACTION_RIGHT;
        case '.': return ACTION_WAIT;
        case 'q': return ACTION_QUIT;
        default:  return ACTION_NIL;
    }
}

void ncurses_render(struct Backend self, RenderEvent cmd) {
    NCursesContext * ctx = self.ctx;
    wmove(ctx->back, cmd.y, cmd.x);
    wattron(ctx->back, COLOR_PAIR(cmd.data.color));
    waddch(ctx->back, cmd.data.ch);
    wattroff(ctx->back, COLOR_PAIR(cmd.data.color));
}

void ncurses_clear_screen(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    werase(ctx->back);
}

void ncurses_begin_rendering(struct Backend self) {
    NCursesContext * ctx = self.ctx;
    ctx->display_i = 0;

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

void ncurses_display(struct Backend self, const char * fmt, ...) {
    NCursesContext * ctx = self.ctx;

    /*move cursor*/
    wmove(ctx->back, ctx->display_i, map_width + 1);
    ctx->display_i += 2;

    va_list args;
    va_start(args, fmt);
    vwprintw(ctx->back, fmt, args);
    va_end(args);
}

Backend backend_init(Allocator a) {
    Backend self = {0};
    NCursesContext * ctx = a.alloc(a, sizeof(NCursesContext));
    if(ctx == NULL) {
        fprintf(stderr, "Memory allocation failure");
        abort();
    }

    self.ctx = ctx;
    self.input = ncurses_input;
    self.render = ncurses_render;
    self.deinit = ncurses_deinit;
    self.finish_rendering = ncurses_finish_rendering;
    self.begin_rendering = ncurses_begin_rendering;
    self.display = ncurses_display;

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
