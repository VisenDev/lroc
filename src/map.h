#ifndef MAP_H
#define MAP_H

#include "entity.h"
#include "pimbs/src/allocator.h"
#include <stdint.h>
#include <assert.h>

/*
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
*/

typedef struct EntityList {
    Entity value;
    struct EntityList * next;
} EntityList;

EntityList * cons (Allocator a, Entity value, EntityList * next) {
    EntityList * self = a.alloc(a, sizeof(EntityList)); 
    assert(self);

    self->next = next;
    self->value = value;
    return self;
}

#define map_width 60
#define map_height 24


typedef struct {
    uint16_t depth;
    uint64_t seed;
} MapCreationSettings;

typedef struct {
    EntityList * data[map_width][map_height];
} Map;

#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)

Map map_create(Allocator a, MapCreationSettings opt) {
    Map result = {0};
    uint8_t x = 0;
    uint8_t y = 0;

    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            if(is_point_on_edge(x, y, map_width, map_height)) {
                Entity wall = immovable_create(a, '#', 253);
                result.data[x][y] = cons(a, wall, NULL);
            }
        }
    }
    return result;
}

void map_deinit(Allocator a, Map map) {
    uint8_t x = 0;
    uint8_t y = 0;

    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            EntityList * ptr = map.data[x][y];
            while(ptr != NULL) {
                EntityList * next = ptr->next;
                Command cmd = {CMD_DEINIT, 0};
                ptr->value.order(ptr->value, a, cmd);
                a.free(a, ptr);
                ptr = next;
            }
        }
    }

}


#endif /*MAP_H*/
