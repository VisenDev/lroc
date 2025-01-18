
#include "backend.h"


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
    short comp[NUM_COMPONENTS][entity_cap];
    int bitmap[NUM_COMPONENTS][entity_cap];
} Scene;

int next(Scene * const self, Id * out, long components) {
    return 0;
}

void render_system(Scene * self) {
    Id id = {0};
    while(next(self, &id, C_X | C_Y | C_CH | C_COLOR)) {
        RenderEvent re = {self->comp[C_X][id.i]};
    }
}
