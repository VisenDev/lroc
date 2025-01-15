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

void render(Backend b, Map * m) {
    uint8_t x = 0;
    uint8_t y = 0;
    static int turn = 0;

    b.begin_rendering(b);
    b.display(b, "Turn: %d", turn);
    turn++;

    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            Cell * ptr = m->data[x][y];
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
}

void update(Backend b, Map * m) {
    uint8_t x = 0;
    uint8_t y = 0;
    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            Cell * ptr = m->data[x][y];
            Cell * prev = ptr;

            while(ptr != NULL) {
                Action action = {0};

                if(!ptr->updated) {
                    ptr->updated = 1;

                    action = ptr->value.update(ptr->value);

                    switch(action) {
                        case ACTION_UP:
                            b.display(b, "Moving up");
                            map_move_entity(m, ptr, x, y, x, y - 1);
                            break;
                        case ACTION_DOWN:
                            b.display(b, "Moving down");
                            map_move_entity(m, ptr, x, y, x, y + 1);
                            break;
                        case ACTION_LEFT:
                            b.display(b, "Moving left");
                            map_move_entity(m, ptr, x, y, x - 1, y);
                            break;
                        case ACTION_RIGHT:
                            b.display(b, "Moving right");
                            map_move_entity(m, ptr, x, y, x + 1, y);
                            break;
                        default: {}
                    }

                    /*ptr->value.order(ptr->value, a, c);*/
                }

                prev = ptr;
                ptr = ptr->next;
            }

        }
    }
}

int main(void) {
    Allocator a = libc_allocator();
    Backend b = backend_init(a);
    MapCreationSettings opt = {0};
    Action input = ACTION_NIL;
    Map m = map_create(a, &input, opt);
    uint8_t x = 0;
    uint8_t y = 0;
    RenderEvent cmd;

    do {
        /*update*/
        update(b, &m);

        /*render*/
        render(b, &m);

        /*get next input*/
        input = b.input(b);


    } while(input != ACTION_QUIT);

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
