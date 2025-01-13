#include "backend/ncurses_backend.c"
#include <stdint.h>

int main(void) {
    Backend b = ncurses_backend();
    InputEvent e;
    RenderEvent cmd;

    do {
        uint8_t x = 0;
        uint8_t y = 0;

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
        e = b.input(b);
    } while (e != EVENT_QUIT);

    return 0;
}
