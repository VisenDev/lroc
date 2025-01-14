#include <stdint.h>

#define USE_STDLIB 1
#define ALLOCATOR_IMPLEMENTATION
#include "pimbs/src/allocator.h"

#define MAP_IMPLEMENTATION
#include "map.h"

#define ENTITY_IMPLEMENTATION
#include "entity.h"

#if   defined(BACKEND_NCURSES)
    #include "ncurses_backend.c"
#elif defined(BACKEND_ANSI)
    #include "ansi_backend.c"
#else
    #warning "No backend specified, defaulting to ANSI"
    #include "ansi_backend.c"
#endif /*BACKEND*/

int main(void) {
    Allocator a = libc_allocator();
    Backend b = backend_init(a);
    MapCreationSettings opt = {0};
    Map m = map_create(a, opt);
    InputEvent event;

    do {
        uint8_t x = 0;
        uint8_t y = 0;
        RenderEvent cmd;

        b.begin_rendering(b);

        for(x = 1; x < 60; ++x) {
            for(y = 1; y < 23; ++y) {
                cmd.data.color = x * y % 256;
                cmd.data.ch = '@';
                cmd.x = x;
                cmd.y = y;
                b.render(b, cmd);
            }
        }

        for(x = 0; x < 80; ++x) {
            for(y = 0; y < 24; ++y) {
                if(x == 0 || y == 0 || x == 79 || y == 23) {
                    cmd.data.color = 244;
                    cmd.data.ch = '#';
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
