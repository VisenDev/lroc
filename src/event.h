#ifndef EVENT_H
#define EVENT_H

typedef enum {
    EVENT_NIL,
    EVENT_WAIT,
    EVENT_UP,
    EVENT_DOWN,
    EVENT_LEFT,
    EVENT_RIGHT,
} Event;

struct EventManager;

typedef Event (*InputFn)(struct EventManager);

typedef struct EventManager {
    void * ctx;
    InputFn get_input;
} EventManager;

#endif /*EVENT_H*/
