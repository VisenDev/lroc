#include "backend.h"
#include "pimbs/src/allocator.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
    RenderData last_frame[map_width][map_height];
} AnsiContext;

InputEvent ansi_input(struct Backend self) {
    const char ch = getchar();
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

void ansi_render(struct Backend self, RenderEvent cmd) {
    AnsiContext * ctx = self.ctx;
    RenderEvent prev = ctx->prev;
    char buffer[256]; 
    int length;

    /*only update if data has changed since last frame*/
    if(memcmp(&ctx->last_frame[cmd.x][cmd.y], &cmd.data, sizeof(RenderData)) == 0) {
        return;
    }

    ctx->last_frame[cmd.x][cmd.y] = cmd.data;
        
    /*move cursor*/
    if(prev.x != cmd.x - 1 || prev.y != cmd.y || prev.data.ch == 0) {
        length = snprintf(buffer, sizeof(buffer), ANSI_ESC "%d;%dH", cmd.y, cmd.x);
        write(STDOUT_FILENO, buffer, length);
    }


    /*adjust color*/
    if(prev.data.color != cmd.data.color || prev.data.ch == 0) {
        length = snprintf(buffer, sizeof(buffer), ANSI_ESC "38;5;%dm", cmd.data.color);
        write(STDOUT_FILENO, buffer, length);
    }

    /*write ch*/
    length = snprintf(buffer, sizeof(buffer), "%c", cmd.data.ch);
    write(STDOUT_FILENO, buffer, length);

    /*
    length = snprintf(buffer, sizeof(buffer), ANSI_COLOR_RESET);
    write(STDOUT_FILENO, ANSI_COLOR_RESET, sizeof(ANSI_COLOR_RESET));
    */
    ctx->prev = cmd;
}


#define GOTO_CURSOR_HOME ANSI_GOTO("0", "24")

void ansi_begin_rendering(struct Backend self) {
    (void)self;
    printf(GOTO_CURSOR_HOME);
    printf(ANSI_CLEAR_LINE);

    fflush(stdout);
}

void ansi_finish_rendering(struct Backend self) {
    AnsiContext * ctx = self.ctx;
    ctx->prev.data.ch = 0;
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

    printf(ANSI_DISABLE_CURSOR);
    /*full buffering*/
    setvbuf(stdout, NULL, _IOFBF, 0);
    printf(ANSI_CLEAR_SCREEN);
    fflush(stdout);

    return self;
}
