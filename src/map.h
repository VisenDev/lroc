#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef struct {
    uint16_t depth
    uint64_t seed;
} GenMapConfig;

struct MapGen;

typedef Map (*GenMapFn)(struct MapGen, GenMapConfig);

typedef struct MapGen {
    void * ctx;
    GenMapFn generate_map;
} MapGen;

#endif /*MAP_H*/
