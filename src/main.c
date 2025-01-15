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
    InputEvent input;
    Map m = map_create(a, &input, opt);
    uint8_t x = 0;
    uint8_t y = 0;
    RenderEvent cmd;

    do {

        /*update*/
        for(x = 0; x < map_width; ++x) {
            for(y = 0; y < map_height; ++y) {
                Cell * ptr = m.data[x][y];
                Cell * prev = ptr;


                while(ptr != NULL) {
                    Command c = {0};
                    if(ptr->updated) continue;
                    ptr->updated = 1;

                    c = ptr->value.update(ptr->value);

                    switch(c.id) {
                        case CMD_UP:
                           map_move_entity(&m, ptr, x, y, x, y - 1);
                           break;
                        case CMD_DOWN:
                           map_move_entity(&m, ptr, x, y, x, y + 1);
                           break;
                        case CMD_LEFT:
                           map_move_entity(&m, ptr, x, y, x - 1, y);
                           break;
                        case CMD_RIGHT:
                           map_move_entity(&m, ptr, x, y, x + 1, y);
                           break;
                        default: {}
                    }

                    /*ptr->value.order(ptr->value, a, c);*/

                    prev = ptr;
                    ptr = ptr->next;
                }

            }
        }


        /*render*/
        b.begin_rendering(b);
        for(x = 0; x < map_width; ++x) {
            for(y = 0; y < map_height; ++y) {
                Cell * ptr = m.data[x][y];
                while(ptr != NULL) {
                    RenderEvent cmd = {0};
                    RenderData data = ptr->value.render(ptr->value);
                    cmd.data = data;
                    cmd.x = x;
                    cmd.y = y;
                    b.render(b, cmd);
                    ptr = ptr->next;
                }
            }
        }
        b.finish_rendering(b);

        /*get next input*/
        input = b.input(b);

    } while(input != INPUT_QUIT);

    b.deinit(b);

    return 0;
}
/*
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
    } while (event != EVENT_QUIT); */
