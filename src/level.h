#ifndef LEVEL_H
#define LEVEL_H

#include "pimbs/src/allocator.h"
#include "backend.h"
#include "bitmap.h"

#define map_width 60
#define map_height 22
#define entity_cap 4096
#define cache_cap 8
#define event_cap 4096


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

typedef struct {
    Entity * listener;
} EventListener;

typedef struct EntityId {
    int i;
} EntityId;

typedef struct EventId{
    int i;
} EventId;

/*Level*/
typedef struct Level {

    /*useful interfaces*/
    Allocator a;
    Backend b;

    /*entities*/
    Entity e[entity_cap];
    Bitmap(entity_cap) entity_bitmap;
    EntityId entity_cache[map_width][map_height][cache_cap]; /*for caching entity locations*/

    /*event queue*/
    Bitmap(event_cap) event_bitmap;
    Event queue[event_cap];
    EventId event_cache[map_width][map_height][cache_cap]; /*for caching event locations*/

    /*
    Entity * location_cache[map_width][map_height][cache_cap];
    EventListener events[num_events][entity_cap];
    */
#endif
} Level;

#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)
#define is_point_within_bounds(x, y, width, height) (x >= 0 && y >= 0 && x < width && y < height)
#define is_point_on_map(x, y) is_point_within_bounds(x, y, map_width, map_height)

/*entity creation functions*/
Entity immovable_create(Level l, short x, short y, char ch, unsigned char color);
Entity decoration_create(Level, char ch, uint8_t color);
Entity player_create(Level, Action*);

#endif /*LEVEL_H*/

#ifdef LEVEL_IMPLEMENTATION

int find_available_entity(Level l) {
    int i = 0;
    for(i = 0; i < entity_cap; ++i) {
        if(get_bit(l.entity_bitmap, i) == 0) {
            return i;
        }
    }
    /*ran out of entities*/
    abort();
}

Level level_create(Allocator a, Backend b, Action * input) {
    Level l = {0};
    short x = 0;
    short y = 0;
    l.a = a;
    l.b = b;

    /*populate entities*/
    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            if(is_point_on_edge(x, y, map_width, map_height)){
                int i = find_available_entity(l);
                set_bit(l.entity_bitmap, i);
                l.e[i] = immovable_create(l, x, y, '#', 243);
            }
        }
    }

    /*spawn player*/
    {
        int i = find_available_entity(l);
        set_bit(l.entity_bitmap, i);
        l.e[i] = player_create(l, input);
    }
    
    return l;
}

/*TODO create level_deinit function*/



#endif /*LEVEL_IMPLEMENTATION*/
#undef LEVEL_IMPLEMENTATION
