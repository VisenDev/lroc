#include "backend.h"
#include "pimbs/src/allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#define ANSI_ESC "\x1b["
#define ANSI_CURSOR_RESET ANSI_ESC "0;0H"
#define ANSI_CLEAR_SCREEN ANSI_ESC "2J"
#define ANSI_CLEAR_LINE ANSI_ESC "2K"

#define ANSI_8BIT_FG(color_code) ANSI_ESC "38;5;" color_code "m"
#define ANSI_8BIT_BG(color_code) ANSI_ESC "48;5;" color_code "m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_GOTO(x, y) ANSI_ESC y ";" x "H"

#define ANSI_DISABLE_CURSOR ANSI_ESC "?25l"
#define ANSI_ENABLE_CURSOR ANSI_ESC "?25h"

typedef struct {
    RenderEvent prev;
    RenderEvent last_frame[map_width][map_height];
    uint8_t display_i;
} AnsiContext;

Action ansi_input(struct Backend self) {
    const char ch = getchar();
    switch(ch) {
        case '\n': return ansi_input(self);
        case 'j': return ACTION_DOWN;
        case 'k': return ACTION_UP;
        case 'h': return ACTION_LEFT;
        case 'l': return ACTION_RIGHT;
        case '.': return ACTION_WAIT;
        case 'q': return ACTION_QUIT;
        default:  return ACTION_NIL;
    }
}

void ansi_render(struct Backend self, RenderEvent cmd) {
    AnsiContext * ctx = self.ctx;
    RenderEvent prev = ctx->prev; /*previous character drawn this frame*/
    RenderEvent last_frame = ctx->last_frame[cmd.x][cmd.y];
    char buffer[256]; 
    int length;

    /*only update if data has changed since last frame*/
    if(last_frame.ch == cmd.ch && last_frame.color == cmd.color){
        return;
    }

    ctx->last_frame[cmd.x][cmd.y] = cmd;
        
    /*move cursor*/
    if(prev.x != cmd.x - 1 || prev.y != cmd.y || prev.ch == 0) {
        length = snprintf(buffer, sizeof(buffer), ANSI_ESC "%d;%dH", cmd.y + 1, cmd.x + 1);
        write(STDOUT_FILENO, buffer, length);
    }


    /*adjust color*/
    if(prev.color != cmd.color || prev.ch == 0) {
        length = snprintf(buffer, sizeof(buffer), ANSI_ESC "38;5;%dm", cmd.color);
        write(STDOUT_FILENO, buffer, length);
    }

    /*write ch*/
    length = snprintf(buffer, sizeof(buffer), "%c", cmd.ch);
    write(STDOUT_FILENO, buffer, length);

    /*
    length = snprintf(buffer, sizeof(buffer), ANSI_COLOR_RESET);
    write(STDOUT_FILENO, ANSI_COLOR_RESET, sizeof(ANSI_COLOR_RESET));
    */
    ctx->prev = cmd;
}


#define GOTO_CURSOR_HOME ANSI_GOTO("0", "23")

void ansi_begin_rendering(struct Backend self) {
    AnsiContext * ctx = self.ctx;
    ctx->display_i = 0;
    printf(GOTO_CURSOR_HOME);
    printf(ANSI_CLEAR_LINE);

    fflush(stdout);
}

void ansi_finish_rendering(struct Backend self) {
    AnsiContext * ctx = self.ctx;
    ctx->prev.ch = 0;
    printf(GOTO_CURSOR_HOME);

    fflush(stdout);
}

void ansi_deinit(struct Backend self) {
    (void)self;
    printf(ANSI_ENABLE_CURSOR);
    printf(ANSI_COLOR_RESET);
    printf(ANSI_CURSOR_RESET);
    printf(ANSI_CLEAR_SCREEN);
    fflush(stdout);
}

void ansi_display(struct Backend self, const char * fmt, ...) {
    AnsiContext * ctx = self.ctx;
    ctx->prev.ch = 0;

    /*move cursor*/
    printf(ANSI_ESC "%d;%dH", ctx->display_i, map_width + 2);
    printf(ANSI_COLOR_RESET);
    ctx->display_i += 1;

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}


Backend backend_init(Allocator a) {
    Backend self = {0};
    AnsiContext * ctx = a.alloc(a, sizeof(AnsiContext));

    if(ctx == NULL) {
        fprintf(stderr, "failure to allocate");
        abort();
    }

    memset(ctx, 0, sizeof(AnsiContext));

    self.ctx = ctx;
    self.input = ansi_input;
    self.render = ansi_render;
    self.deinit = ansi_deinit;
    self.finish_rendering = ansi_finish_rendering;
    self.begin_rendering = ansi_begin_rendering;
    self.display = ansi_display;

    printf(ANSI_DISABLE_CURSOR);
    /*full buffering*/
    setvbuf(stdout, NULL, _IOFBF, 0);
    printf(ANSI_CLEAR_SCREEN);
    fflush(stdout);

    return self;
}
