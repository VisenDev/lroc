#include <stdint.h>
#include <assert.h>
#include "level.h"

typedef struct {
    RenderEvent event;
} ImmovableContext;

void immovable_render(struct Entity self, struct Level l) {
    ImmovableContext* ctx = self.ctx;
    l.b.render(l.b, ctx->event);
}

void immovable_update(struct Entity self, struct Level l) {
    (void)self;
    (void)l;
}

void immovable_destroy(struct Entity self, struct Level l) {
    l.a.free(l.a, self.ctx); 
}


void immovable_notify(struct Entity self, struct Level l, struct Event event) {


}

Entity immovable_create(Level l, short x, short y, char ch, unsigned char color) {
    Entity self = {0};
    ImmovableContext * ctx = l.a.alloc(l.a, sizeof(ImmovableContext));
    assert(ctx);

    ctx->event.ch = ch;
    ctx->event.color = color;
    ctx->event.x = x;
    ctx->event.y = y;
    
    self.ctx = ctx;

    self.render = immovable_render;
    self.update = immovable_update;
    self.notify = immovable_notify;
    self.destroy = immovable_destroy;

    return self;
}


/*Player*/
typedef struct {
    short health;
    short x;
    short y;
    Action * input_ptr;
} PlayerContext;

void player_render(struct Entity self, struct Level l) {
    PlayerContext * ctx = self.ctx;
    RenderEvent event = {0};
    event.ch = '@';
    event.color = 25;
    event.x = ctx->x;
    event.y = ctx->y;
    l.b.render(l.b, event);
}

void player_update(struct Entity self, struct Level l) {
    PlayerContext * ctx = self.ctx;
    Action action = *ctx->input_ptr;

    /*TODO implement motion*/
}

void player_destroy(struct Entity self, struct Level l) {
    l.a.free(l.a, self.ctx);
}


void player_notify(struct Entity self, struct Level l, struct Event e) {
    /*TODO implement*/
}

Entity player_create(Level l, Action * input_ptr) {
    Entity self = {0};
    PlayerContext * ctx = l.a.alloc(l.a, sizeof(PlayerContext));
    assert(ctx);

    ctx->input_ptr = input_ptr;
    ctx->health = 10;
    ctx->x = 5;
    ctx->y = 5;
    
    self.ctx = ctx;
    self.render = player_render;
    self.update = player_update;
    self.destroy = player_destroy;

    return self;
}
