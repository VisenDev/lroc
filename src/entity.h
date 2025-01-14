#ifndef ENTITY_H
#define ENTITY_H

#include "backend.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int impassable;
} EntityQuery;

struct Entity;

typedef RenderEvent (*EntityRenderFn)(struct Entity);
typedef void (*EntityUpdateFn)(struct Entity);
typedef EntityQuery (*EntityQueryFn)(struct Entity);

typedef struct Entity {
    void * ctx;
    EntityRenderFn render;
    EntityUpdateFn update;
    EntityQueryFn query;
} Entity;

#endif /*ENTITY_H*/
