#ifndef BACKEND_H
#define BACKEND_H

#include <stdint.h>
#include "pimbs/src/allocator.h"


/*data types*/
typedef enum {
    EVENT_NIL,
    EVENT_WAIT,
    EVENT_UP,
    EVENT_DOWN,
    EVENT_LEFT,
    EVENT_RIGHT,
    EVENT_QUIT
} InputEvent;

typedef struct {
    char ch;
    uint8_t x;
    uint8_t y;
    uint8_t color; /*ansi terminal 256-color code*/
} RenderEvent;


struct Backend;

/*Function pointer types*/
typedef InputEvent (*InputFn)(struct Backend);
typedef void       (*RenderFn)(struct Backend, RenderEvent);
typedef void       (*FinishRenderingFn)(struct Backend);
typedef void       (*BeginRenderingFn)(struct Backend);
typedef void       (*ClearScreenFn)(struct Backend);
typedef void       (*DeinitFn)(struct Backend);

/*interface*/
typedef struct Backend {
    void * ctx;
    InputFn input;
    RenderFn render;
    ClearScreenFn clear_screen;
    FinishRenderingFn finish_rendering;
    BeginRenderingFn begin_rendering;
    DeinitFn deinit;
} Backend;

/*init function (defined by each backend) */
Backend backend_init(Allocator);

#endif /*BACKEND_H*/
