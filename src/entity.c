#include <stdbool.h>
#include <assert.h>
#define ALLOCATOR_IMPLEMENTATION
#define USE_STDLIB 1
#include "pimbs/src/allocator.h"
#include "backend.h"

/*macros*/
#define entity_cap 4096
#define map_width 60
#define map_height 22


#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)
#define is_point_within_bounds(x, y, width, height) (x >= 0 && y >= 0 && x < width && y < height)
#define is_point_on_map(x, y) is_point_within_bounds(x, y, map_width, map_height)

static int valid_destination(short x, short y) {
    return is_point_on_map(x, y);
}

typedef enum {
    player,
    rat,
    decoration,
} EntityType;

typedef struct Entity {
    EntityType type;

    short x;
    short y;
    char ch;
    short color;

    short health;
    bool block_movement;
} Entity;

typedef struct {
    bool entity_used[entity_cap];
    Entity entity[entity_cap];
} Scene;

typedef struct {
    short i;
} Id;
const Id null_id = {-1};

typedef struct {
    Allocator a;
    Backend b;    
    Action input;
} RunTime;

Id new_entity(Scene * s) {
    unsigned int i = 0;
    for(i = 0; i < entity_cap; ++i) {
        if(s->entity_used[i] == false) {
            const Id result = {i};
            s->entity_used[i] = true;
            return result;
        }
    }
    return null_id;
}

void delete_entity(Scene * s, Id id) {
    assert(id.i >= 0);
    assert(id.i < entity_cap);
    s->entity_used[id.i] = false;
}

Entity * get_entity(Scene * s, Id id) {
    assert(id.i >= 0);
    assert(id.i < entity_cap);
    return &s->entity[id.i];
}

/*entity implementations*/



/*specific*/
void update_player(Scene * s, RunTime * r, Id id) {
    Entity * entity = get_entity(s, id);
    const Action action = r->input;
    switch(action) {
        case ACTION_UP: 
            if(valid_destination(entity->x, entity->y - 1)) {
                entity->y -= 1;
            }
            break;
        case ACTION_DOWN: 
            if(valid_destination(entity->x, entity->y + 1)) {
                entity->y += 1;
            }
            break;
        case ACTION_LEFT: 
            if(valid_destination(entity->x - 1, entity->y)) {
                entity->x -= 1;
            }
            break;
        case ACTION_RIGHT: 
            if(valid_destination(entity->x + 1, entity->y)) {
                entity->x += 1;
            }
            break;
        default:
            break;
    }
}


/*generic*/
void render(Scene * s, RunTime * r, Id id) {
    const Entity * entity = get_entity(s, id);
    const RenderEvent event = {entity->x, entity->y, entity->ch, entity->color};
    r->b.render(r->b, event);
}

void update(Scene * s, RunTime * r, Id id) {
    const Entity * entity = get_entity(s, id);
    switch(entity->type) {
        case player: update_player(s, r, id);
        case rat: {}
        case decoration: {}
    }
}

Scene create_scene(void) {
    short x = 0;
    short y = 0;
    Scene result = {0};
    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            if(is_point_on_edge(x, y, map_width, map_height)){
                Id id = new_entity(&result);
                Entity * entity = get_entity(&result, id);
                entity->x = x;
                entity->y = y;
                entity->ch = '#';
                entity->color = 200;
                entity->block_movement = true;
            } else {
                Id id = new_entity(&result);
                Entity * entity = get_entity(&result, id);
                entity->x = x;
                entity->y = y;
                entity->ch = '.';
                entity->color = 20;
                entity->block_movement = false;
            }
        }
    }
    return result;
}

#include "ansi_backend.c"

/* entry point */
int main(void) {
    Allocator a = libc_allocator();
    Backend b = backend_init(a);
    RunTime r = {a, b, ACTION_NIL};
    Scene s = create_scene();

    while(r.input != ACTION_QUIT) {
        unsigned int i = 0;
        /*update*/
        for(i = 0; i < entity_cap; ++i) {
            const Id id = {i};
            if(s.entity_used[i]) {
                update(&s, &r, id);
            }
        }

        for(i = 0; i < entity_cap; ++i) {
            const Id id = {i};
            if(s.entity_used[i]) {
                render(&s, &r, id);
            }
        }

        r.input = r.b.input(r.b);
    }

    return 0;
}
