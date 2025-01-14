#include <stdint.h>

#include "map.h"
#include "entity.h"
#define MAX_ENTITIES 4096

#define USE_STDLIB 1
#define ALLOCATOR_IMPLEMENTATION
#include "pimbs/src/allocator.h"

#include "ncurses_backend.c"
/*#include "ansi_backend.c"*/

#include "simple_map.c"

int main(void) {
    Allocator a = libc_allocator();
    Backend b = ncurses_backend(a);
    MapGenerator g = simple_map_generator();
    InputEvent event;

    static Entity ent[MAX_ENTITIES];
    uint32_t num_ent = 0;

    do {
        uint8_t x = 0;
        uint8_t y = 0;
        RenderEvent cmd;

        b.begin_rendering(b);
        b.clear_screen(b);

        for(x = 1; x < 60; ++x) {
            for(y = 1; y < 23; ++y) {
                cmd.color = x * y % 256;
                cmd.ch = '@';
                cmd.x = x;
                cmd.y = y;
                b.render(b, cmd);
            }
        }

        for(x = 0; x < 80; ++x) {
            for(y = 0; y < 24; ++y) {
                if(x == 0 || y == 0 || x == 79 || y == 23) {
                    cmd.color = 244;
                    cmd.ch = '#';
                    cmd.x = x;
                    cmd.y = y;
                    b.render(b, cmd);
                }
            }
        }

        b.finish_rendering(b);
        event = b.input(b);
    } while (event != EVENT_QUIT);

    b.deinit(b);

    return 0;
}
