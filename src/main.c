#include <stdint.h>

#define USE_STDLIB 1
#define ALLOCATOR_IMPLEMENTATION
#include "pimbs/src/allocator.h"

#include "level.h"
#include "entity_implementations.c"

#if defined(BACKEND_NCURSES)
    #include "ncurses_backend.c"
#elif defined(BACKEND_ANSI)
    #include "ansi_backend.c"
#else
    #warning "No backend specified, defaulting to ANSI"
    #include "ansi_backend.c"
#endif /*BACKEND*/

#if 0
void render(Backend b, Map * m) {
    short x = 0;
    short y = 0;
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

void update_cell(Backend b, Map * m, short x, short y) {
    Cell * ptr = m->data[x][y];
    Cell * prev = NULL;

    while(ptr != NULL) {
        Action action = {0};
        action = ptr->value.update(ptr->value);

        switch(action) {
            case ACTION_UP:
                if(is_point_on_map(x, y - 1)) {
                    b.display(b, "Moving %p up", ptr);
                    m->data[x][y - 1] = cell_append(m->data[x][y - 1], cell_remove(prev, ptr)); 
                }
                break;
            case ACTION_DOWN:
                if(is_point_on_map(x, y + 1)) {
                    b.display(b, "Moving %p down", ptr);
                    m->data[x][y + 1] = cell_append(m->data[x][y + 1], cell_remove(prev, ptr)); 
                }
                break;
            case ACTION_LEFT:
                if(is_point_on_map(x - 1, y)) {
                    b.display(b, "Moving %p left", ptr);
                    m->data[x - 1][y] = cell_append(m->data[x - 1][y], cell_remove(prev, ptr)); 
                }
                break;
            case ACTION_RIGHT:
                if(is_point_on_map(x + 1, y)) {
                    b.display(b, "Moving %p right", ptr);
                    m->data[x + 1][y] = cell_append(m->data[x + 1][y], cell_remove(prev, ptr)); 
                }
                break;
            default: {}
        }

        /*ptr->value.order(ptr->value, a, c);*/

        prev = ptr;
        ptr = ptr->next;

    }
}

void update(Backend b, Map * m) {
    short x = 0;
    short y = 0;
    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            update_cell(b, m, x, y);
        }
    }
}
#endif

int main(void) {
    Allocator a = libc_allocator();
    Backend b = backend_init(a);
    Action input = ACTION_NIL;
    Level l = {0};
    short x = 0;
    short y = 0;
    RenderEvent cmd;

    do {
        /*update*/

        /*render*/

        /*get next input*/
        input = b.input(b);


    } while(input != ACTION_QUIT);

    b.deinit(b);

    return 0;
}
/*
   do {
   short x = 0;
   short y = 0;
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
