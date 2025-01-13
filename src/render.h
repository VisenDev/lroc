#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

typedef struct {
    char ch;
    uint8_t x;
    uint8_t y;
    uint8_t color; /*ansi terminal 256-color code*/
} RenderCommand;


struct Renderer;

typedef void (*RenderFn)(struct Renderer, RenderCommand);
typedef void (*ClearFn)(struct Renderer);

typedef struct Renderer {
    void * ctx;
    ClearFn clear_screen;
    RenderFn render;
} Renderer;


#endif /*RENDER_H*/
