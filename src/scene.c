
#define ALLOCATOR_IMPLEMENTATION
#define USE_STDLIB 1
#include "pimbs/src/allocator.h"
#include "pimbs/src/bitmap.h"
#include "backend.h"
#include <assert.h>

#include "ansi_backend.c"


#define entity_cap 2048
#define map_width 60
#define map_height 22
#define is_point_within_bounds(x, y, width, height) (x >= 0 && y >= 0 && x < width && y < height)
#define is_point_on_map(x, y) is_point_within_bounds(x, y, map_width, map_height)
#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)


#define component_count 8

#define COMP_X 0
#define COMP_Y 1
#define COMP_COLOR 2
#define COMP_CH 3
#define COMP_HEALTH 4
#define COMP_IS_PLAYER 5
#define COMP_FOREGROUND 6


typedef struct Scene {
    Backend b;
    Action * input;
    Bitmap(entity_cap) free_entities;
    Bitmap2D(component_count, entity_cap) bitmap;
    short c[component_count][entity_cap];
    Bitmap2D(map_width, map_height) map;
} Scene;


typedef void (*SystemFn) (Scene *, short);



void foreach_entity(Scene * self, short * components, short len, SystemFn func) {
    short entity = 0;
    for(entity = 0; entity < entity_cap; ++entity) {
        short i = 0;
        short num_matches = 0;
        for(i= 0; i < len; ++i) {
            int result = get_bit_2d(self->bitmap, components[i], entity);
            assert(result == 0 || result == 1);
            num_matches += result;
        }
        if(num_matches == len) {
            func(self, entity);
        }
    }
}


short find_free_entity(Scene * self) {
    short i = 0;
    for(i = 0; i < entity_cap; ++i) {
        if(get_bit(self->free_entities, i) == 0) {
            return i;
        }
    }
    /*ran out of entities*/
    return -1;
}

void set_component(Scene * self, short component, short entity, short value) {
    set_bit_2d(self->bitmap, component, entity);
    self->c[component][entity] = value;
}


/*systems*/
void render_system(Scene * self, short entity) {
    RenderEvent re = {0};
    re.ch = (char)self->c[COMP_CH][entity];
    re.x = self->c[COMP_X][entity];
    re.y = self->c[COMP_Y][entity];
    re.color = self->c[COMP_COLOR][entity];
    self->b.render(self->b, re);
}

void player_system(Scene * self, short entity) {
    switch(*self->input) {
        default: break;
    }
}

/*entity creation*/
void create_player(Scene * self) {
    short id = find_free_entity(self);
    assert(id >= 0);
    set_component(self, COMP_X, id, 5);
    set_component(self, COMP_Y, id, 5);
    set_component(self, COMP_CH, id, (short)'@');
    set_component(self, COMP_COLOR, id, 52);
    set_component(self, COMP_IS_PLAYER, id, 0);
    set_component(self, COMP_FOREGROUND, id, 0);
}


void create_decoration(Scene * self, short x, short y, char ch, short color) {
    short id = find_free_entity(self);
    assert(id >= 0);
    set_component(self, COMP_X, id, x);
    set_component(self, COMP_Y, id, y);
    set_component(self, COMP_CH, id, (short)ch);
    set_component(self, COMP_COLOR, id, color);
}

/*scene creation*/
void create_scene(Scene * self, Backend b, Action * input) {
    short x, y = 0;

    self->b = b;
    self->input = input;

    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_width; ++y) {
            if(is_point_on_edge(x, y, map_width, map_height)) {
                create_decoration(self, x, y, '#', 252);
                /*set_bit_2d(self->map, x, y);*/ /*TODO fix*/
            } else {
                create_decoration(self, x, y, '.', 47);
                /*unset_bit_2d(self->map, x, y);*/
            }
        }
    }

    create_player(self);

}

#define array_len(array) (sizeof(array) / sizeof(array[0]))

int main() {
    Allocator a = libc_allocator();
    Backend b = backend_init(a);
    Action input = ACTION_NIL;
    static Scene s = {0};
    create_scene(&s, b, &input);
    int i = 0;

    while(input != ACTION_QUIT) {
        {
            short components[] = {COMP_IS_PLAYER};
            foreach_entity(&s, components, array_len(components), player_system);
        }
        {
            short components[] = {COMP_X, COMP_Y, COMP_CH, COMP_COLOR};
            foreach_entity(&s, components, array_len(components), render_system);
        }
        {
            short components[] = {COMP_X, COMP_Y, COMP_CH, COMP_COLOR, COMP_FOREGROUND};
            foreach_entity(&s, components, array_len(components), render_system);
        }
        ++i;
        input = b.input(b);
    }

    return 0;
}
