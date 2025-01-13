#include <stdint.h>

#include "game/map.h"
#include "game/entity.h"
#define MAX_ENTITIES 4096


/*
#include "backend/ncurses_backend.c"
*/
#include "backend/ansi_backend.c"
#include "game/simple_map.c"

int main(void) {
    Backend b = BACKEND();
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

        for(x = 0; x < 80; ++x) {
            for(y = 0; y < 24; ++y) {
                cmd.color = x;
                cmd.ch = '@';
                cmd.x = x;
                cmd.y = y;
                b.render(b, cmd);
            }
        }

        b.finish_rendering(b);
        event = b.input(b);
    } while (event != EVENT_QUIT);

    return 0;
}
