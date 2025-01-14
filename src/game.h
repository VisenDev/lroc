#ifndef GAME_H
#define GAME_H

#include "../backend/backend.h"
#include <stdbool.h>
#include <stdint.h>

/*map*/
typedef struct {
    uint16_t depth;
    uint64_t seed;
} MapConfig;

struct MapGenerator;

typedef void (*GenerateMapFn)(struct MapGenerator, MapConfig, Entity[], uint32_t);

typedef struct MapGenerator {
    void * ctx;
    GenerateMapFn generate_map;
} MapGenerator;

/*entity*/
typedef struct {
    bool impassable;
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

#endif /*GAME_H*/
