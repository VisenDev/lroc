#include "backend.h"
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>

/*
#define C_X (1<<0)
#define C_Y (1<<1)
#define C_CH (1<<2)
#define C_COLOR (1<<3)
#define C_HEALTH (1<<4)
#define C_PLAYER (1<<5)
#define C_FOREGROUND (1<<6)

#define NUM_COMPONENTS 7
*/

#define C_X 0
#define C_Y 1
#define C_CH 2
#define C_COLOR 3
#define C_IS_HEALTH 4
#define C_PLAYER 5
#define C_RENDER_FOREGROUND 6
#define NUM_COMPONENTS 7

#define entity_cap 4096

typedef struct {
    short i;
} Id;

typedef struct {
    bool entities[entity_cap];
    long long component[NUM_COMPONENTS][entity_cap];
    bool component_bitmap[NUM_COMPONENTS][entity_cap];
} Scene;

/*
long component_to_index(long component) {
    int i = 0;
    for(i = 0; i < sizeof(long) * CHAR_BIT; ++i) {
        if(component & (1<<i)) {
            return i;        
        }
    }
    return -1;
}*/

short get_component(Scene * self, long component_id, Id entity) {
    return self->component[component_id][entity.i];
}


int next(Scene * const self, Id * out, long components) {
    return 0;
}

void render_system(Scene * self) {
    Id id = {0};
    while(next(self, &id, C_X | C_Y | C_CH | C_COLOR)) {
        RenderEvent re = {self->component[C_X][id.i]};
    }
}
