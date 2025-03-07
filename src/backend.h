#ifndef BACKEND_H
#define BACKEND_H

#include <stdint.h>
#include "pimbs/src/allocator.h"

typedef enum {
    ACTION_NIL,
    ACTION_WAIT,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_QUIT
} Action;


typedef struct {
    short x;
    short y;
    char ch;
    unsigned char color; /*ansi terminal 256-color code*/
} RenderEvent;


struct Backend;

/*Function pointer types*/
typedef Action     (*InputFn)(struct Backend);
typedef void       (*RenderFn)(struct Backend, RenderEvent);
typedef void       (*FinishRenderingFn)(struct Backend);
typedef void       (*BeginRenderingFn)(struct Backend);
typedef void       (*DeinitFn)(struct Backend);
typedef void       (*DisplayFn)(struct Backend, const char * fmt, ...);

/*interface*/
typedef struct Backend {
    void * ctx;
    InputFn input;
    RenderFn render;
    FinishRenderingFn finish_rendering;
    BeginRenderingFn begin_rendering;
    DeinitFn deinit;
    DisplayFn display;
} Backend;

/*init function (defined by each backend) */
Backend backend_init(Allocator);

#endif /*BACKEND_H*/
