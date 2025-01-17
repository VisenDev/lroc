#include "backend.h"
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>

#define C_X (1<<0)
#define C_Y (1<<1)
#define C_CH (1<<2)
#define C_COLOR (1<<3)
#define C_HEALTH (1<<4)
#define C_PLAYER (1<<5)
#define C_FOREGROUND (1<<6)

#define NUM_COMPONENTS 7

#define entity_cap 1024

typedef struct {
    short i;
} Id;

typedef struct {
    bool entities[entity_cap];
    short component[NUM_COMPONENTS][entity_cap];
    bool component_bitmap[NUM_COMPONENTS][entity_cap];
} Scene;

long component_to_index(long component) {
    int i = 0;
    for(i = 0; i < sizeof(long) * CHAR_BIT; ++i) {
        if(component & (1<<i)) {
            return i;        
        }
    }
    return -1;
}

short get_component(Scene * self, Id entity, long component) {
    long index =  component
}


int next(Scene * const self, Id * out, long components) {
    return 0;
}

void render_system(Scene * self) {
    Id id = {0};
    while(next(self, &id, C_X | C_Y | C_CH | C_COLOR)) {
        RenderEvent re = {self->comp[C_X][id.i]};
    }
}
