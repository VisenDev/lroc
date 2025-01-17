#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include "level.h"
#include "pimbs/src/bitmap.h"

typedef struct {
    RenderEvent event;
} ImmovableContext;

void decoration_render(struct Entity self, struct Level * l) {
    ImmovableContext* ctx = self.ctx;
    l->b.render(l->b, ctx->event);
}

void decoration_update(struct Entity self, struct Level * l) {
    (void)self;
    (void)l;
}

void decoration_destroy(struct Entity self, struct Level*  l) {
    l->a.free(l->a, self.ctx); 
}


void decoration_notify(struct Entity self, struct Level * l,  struct Event event) {
    
}

Entity decoration_create(Level l, short x, short y, char ch, unsigned char color) {
    Entity self = {0};
    ImmovableContext * ctx = l.a.alloc(l.a, sizeof(ImmovableContext));
    assert(ctx);

    ctx->event.ch = ch;
    ctx->event.color = color;
    ctx->event.x = x;
    ctx->event.y = y;
    
    self.ctx = ctx;

    self.render = decoration_render;
    self.update = decoration_update;
    self.notify = decoration_notify;
    self.destroy = decoration_destroy;

    return self;
}


/*Player*/
typedef struct {
    short health;
    short x;
    short y;
    Action * input_ptr;
} PlayerContext;

void player_render(struct Entity self, struct Level * l) {
    PlayerContext * ctx = self.ctx;
    RenderEvent event = {0};
    event.ch = '@';
    event.color = 172;
    event.x = ctx->x;
    event.y = ctx->y;
    l->b.render(l->b, event);
}

void player_update(struct Entity self, struct Level * l) {
    PlayerContext * ctx = self.ctx;
    Action action = *ctx->input_ptr;

    switch(action) {
        case ACTION_UP: 
            if(valid_destination(l, ctx->x, ctx->y - 1)) {
                ctx->y -= 1;
            }
            break;
        case ACTION_DOWN: 
            if(valid_destination(l, ctx->x, ctx->y + 1)) {
                ctx->y += 1;
            }
            break;
        case ACTION_LEFT: 
            if(valid_destination(l, ctx->x - 1, ctx->y)) {
                ctx->x -= 1;
            }
            break;
        case ACTION_RIGHT: 
            if(valid_destination(l, ctx->x + 1, ctx->y)) {
                ctx->x += 1;
            }
            break;
        default:
            break;
    }
}

void player_destroy(struct Entity self, struct Level * l) {
    l->a.free(l->a, self.ctx);
}


void player_notify(struct Entity self, struct Level * l, struct Event e) {
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





/*Rat*/
typedef struct {
    short health;
    short x;
    short y;
} RatContext;

void rat_render(struct Entity self, struct Level * l) {
    RatContext * ctx = self.ctx;
    RenderEvent event = {0};
    event.ch = 'r';
    event.color = 88;
    event.x = ctx->x;
    event.y = ctx->y;
    l->b.render(l->b, event);
}

void rat_update(struct Entity self, struct Level * l) {
    RatContext * ctx = self.ctx;
    Action action = rand() % 6;

    switch(action) {
        case ACTION_UP: 
            if(valid_destination(l, ctx->x, ctx->y - 1)) {
                ctx->y -= 1;
            }
            break;
        case ACTION_DOWN: 
            if(valid_destination(l, ctx->x, ctx->y + 1)) {
                ctx->y += 1;
            }
            break;
        case ACTION_LEFT: 
            if(valid_destination(l, ctx->x - 1, ctx->y)) {
                ctx->x -= 1;
            }
            break;
        case ACTION_RIGHT: 
            if(valid_destination(l, ctx->x + 1, ctx->y)) {
                ctx->x += 1;
            }
            break;
        default:
            break;
    }
}

void rat_destroy(struct Entity self, struct Level * l) {
    l->a.free(l->a, self.ctx);
}


void rat_notify(struct Entity self, struct Level * l, struct Event e) {
    /*TODO implement*/
}

Entity rat_create(Level l, short x, short y) {
    Entity self = {0};
    RatContext * ctx = l.a.alloc(l.a, sizeof(RatContext));
    assert(ctx);

    ctx->health = 10;
    ctx->x = x;
    ctx->y = y;
    
    self.ctx = ctx;
    self.render = rat_render;
    self.update = rat_update;
    self.destroy = rat_destroy;

    return self;
}
