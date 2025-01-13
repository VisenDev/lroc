#ifndef MAP_H
#define MAP_H

typedef struct {

} GenMapConfig;

struct MapGen;

typedef (*GenMapFn)(struct MapGen, GenMapConfig);

typedef struct MapGen {
    void * ctx;
    GenMapFn generate_map;
} MapGen;

#endif /*MAP_H*/
