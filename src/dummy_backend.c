#include "backend.h"
#include "pimbs/src/allocator.h"

Action dummy_input(struct Backend self) {
    (void)self;
    return ACTION_LEFT;
}

void dummy_render(struct Backend self, RenderEvent cmd) {
    (void)self;
    (void)cmd;
}

void dummy_begin_rendering(struct Backend self) {
    (void)self;
}

void dummy_finish_rendering(struct Backend self) {
    (void)self;
}

void dummy_deinit(struct Backend self) {
    (void)self;
}

void dummy_display(struct Backend self, const char * fmt, ...) {
    (void)self;
    (void)fmt;
}

Backend backend_init(Allocator a) {
    Backend self = {0};
    (void)a;

    self.ctx = NULL;
    self.input = dummy_input;
    self.render = dummy_render;
    self.deinit = dummy_deinit;
    self.finish_rendering = dummy_finish_rendering;
    self.begin_rendering = dummy_begin_rendering;
    self.display = dummy_display;

    return self;
}
