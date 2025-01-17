#ifndef LEVEL_H
#define LEVEL_H

#include <stdlib.h>
#include "pimbs/src/allocator.h"
#include "backend.h"
#include "pimbs/src/bitmap.h"

#define map_width 60
#define map_height 22
#define entity_cap 4096
#define cache_cap 8
#define event_cap 4096


struct Entity;
struct Event;
struct Level;


/*Entity*/
typedef void (*EntityRenderFn) (struct Entity, struct Level *);
typedef void (*EntityUpdateFn) (struct Entity, struct Level *);
typedef void (*EntityDestroyFn)(struct Entity, struct Level *);
typedef void (*EntityNotifyFn) (struct Entity, struct Level *, struct Event);

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
    EventId type;
    int value;
    Entity * sender;
} Event;

typedef struct {
    Entity * listener;
} EventListener;

typedef long Id;

/*Level*/
typedef struct Level {

    /*useful interfaces*/
    Allocator a;
    Backend b;

    /*mobility bitmap*/
    Bitmap2D(map_width, map_height) map;

    /*entities*/
    Entity e[entity_cap];
    Bitmap(entity_cap) entity_bitmap;
    Id entity_cache[map_width][map_height][cache_cap]; /*for caching entity locations*/

    /*event queue*/
    Bitmap(event_cap) event_bitmap;
    Event queue[event_cap];
    Id event_cache[map_width][map_height][cache_cap]; /*for caching event locations*/

    /*
    Entity * location_cache[map_width][map_height][cache_cap];
    EventListener events[num_events][entity_cap];
    */
} Level;


#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)
#define is_point_within_bounds(x, y, width, height) (x >= 0 && y >= 0 && x < width && y < height)
#define is_point_on_map(x, y) is_point_within_bounds(x, y, map_width, map_height)


static int valid_destination(Level * l, short x, short y) {
    return is_point_on_map(x, y) && !get_bit_2d(l->map, x, y);
}

/*entity creation functions*/
Entity decoration_create(Level l, short x, short y, char ch, unsigned char color);
/*Entity decoration_create(Level, char ch, uint8_t color);*/
Entity player_create(Level, Action*);
Entity rat_create(Level l, short, short);

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
                l.e[i] = decoration_create(l, x, y, '#', 243);
                set_bit_2d(l.map, x, y);
            } else {
                int i = find_available_entity(l);
                set_bit(l.entity_bitmap, i);
                l.e[i] = decoration_create(l, x, y, '.', 138);
                unset_bit_2d(l.map, x, y);

                if(!(rand() % 25)) {
                    i = find_available_entity(l); 
                    set_bit(l.entity_bitmap, i);
                    l.e[i] = rat_create(l, x, y);
                }
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
