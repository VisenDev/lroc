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

typedef struct Cell {
    Entity value;
    int updated;
    struct Cell * next;
} Cell;

Cell * cell_cons(Allocator a, Entity value, Cell * next) {
    Cell * self = a.alloc(a, sizeof(Cell)); 
    assert(self);
    self->next = next;
    self->value = value;
    return self;
}

/*returns start*/
Cell * cell_append(Cell * start, Cell * ptr) {
    if(start == NULL) {
        return ptr;
    }
    while(start->next != NULL) {
        start = start->next;
    }
    start->next = ptr;
    ptr->next = NULL;
}

Cell * cell_remove(Cell * prev, Cell * ptr) {
    assert(ptr);
    if(prev) {
        ptr->next = ptr->next;
    }
    ptr->next = NULL;
    return ptr;
}

/*IMPORTANT MACROS*/

#define map_width 60
#define map_height 24

#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)
#define is_point_within_bounds(x, y, width, height) (x >= 0 && y >= 0 && x < width && y < height)
#define is_point_on_map(x, y) is_point_within_bounds(x, y, map_width, map_height)

/*TYPES*/

typedef struct {
    uint16_t depth;
    uint64_t seed;
} MapCreationSettings;

typedef struct {
    Cell * player;
    Cell * data[map_width][map_height];
} Map;


typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

void map_move_entity(
        Map * map,
        Cell * entity,
        short old_x,
        short old_y,
        short new_x,
        short new_y) {
    Cell * ptr = map->data[old_x][old_y];
    Cell * prev = NULL;
    assert(entity != NULL);

    fprintf(stderr, "moving entity %p from %d %d to %d %d\n", entity, old_x, old_y, new_x, new_y);

    if(!is_point_within_bounds(new_x, new_y, map_width, map_height)) {
       return; 
    }

    while(ptr != entity) {
        assert(ptr != NULL);

        prev = ptr;
        ptr = ptr->next;
    }

    assert(ptr == entity);

    /*remove from old location*/
    prev->next = ptr->next;
    entity->next = NULL;

    /*append to new location*/
    ptr = map->data[new_x][new_y];
    while(ptr != NULL) {
        prev = ptr;
        ptr = ptr->next;
    }
    prev->next = entity;
}


Map map_create(Allocator a, Action * input, MapCreationSettings opt) {
    Map result = {0};
    short x = 0;
    short y = 0;

    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            if(is_point_on_edge(x, y, map_width, map_height)) {
                Entity wall = immovable_create(a, '#', 253);
                result.data[x][y] = cell_cons(a, wall, NULL);
            }
        }
    }

    Entity player = player_create(a, input);
    result.data[5][5] = cell_cons(a, player, NULL);
    result.player = result.data[5][5];
    return result;
}

void map_deinit(Allocator a, Map map) {
    short x = 0;
    short y = 0;

    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            Cell * ptr = map.data[x][y];
            while(ptr != NULL) {
                Cell * next = ptr->next;
                ptr->value.destroy(ptr->value, a);
                a.free(a, ptr);
                ptr = next;
            }
        }
    }

}


#endif /*MAP_H*/
