#ifndef LEVEL_H
#define LEVEL_H

#include "pimbs/src/allocator.h"
#include "backend.h"

#define map_width 60
#define map_height 22
#define entity_cap 4096
#define cache_cap 8


struct Entity;
struct Event;
struct Level;


/*Entity*/
typedef void (*EntityRenderFn) (struct Entity, struct Level);
typedef void (*EntityUpdateFn) (struct Entity, struct Level);
typedef void (*EntityDestroyFn)(struct Entity, struct Level);
typedef void (*EntityNotifyFn) (struct Entity, struct Level, struct Event);

typedef struct Entity {
    void * ctx;
    EntityRenderFn render;
    EntityUpdateFn update;
    EntityDestroyFn destroy;
    EntityNotifyFn notify;
} Entity;



/*Event system*/
#define num_events 4
typedef enum {
    EVENT_NIL,
    EVENT_IGNORED,
    EVENT_OVERLAP,
    EVENT_HEALTH,
    EVENT_BLOCK_MOVEMENT
} EventId;

typedef struct Event {
    EventId id;
    int value;
    Entity * sender;
} Event;

/*Level*/
typedef struct Level {
    Allocator a;
    Backend b;
    int entity_bitmap[entity_cap];
    Entity entity[entity_cap];
    Entity * location_cache[map_width][map_height][cache_cap];
    Entity * listeners[num_events][entity_cap];
} Level;

#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)
#define is_point_within_bounds(x, y, width, height) (x >= 0 && y >= 0 && x < width && y < height)
#define is_point_on_map(x, y) is_point_within_bounds(x, y, map_width, map_height)

/*entity creation functions*/
Entity immovable_create(Level l, short x, short y, char ch, char color);
Entity decoration_create(Level, char ch, uint8_t color);
Entity player_create(Level, Action*);

#endif /*LEVEL_H*/
