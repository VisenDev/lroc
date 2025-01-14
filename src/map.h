#ifndef MAP_H
#define MAP_H

#include "entity.h"
#include <stdint.h>

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

#endif /*MAP_H*/
