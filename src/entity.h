#ifndef ENTITY_H
#define ENTITY_H

#include "backend.h"
#include <stdint.h>
#include <assert.h>


struct Entity;

typedef enum {
    ORDER_IGNORED,
    ORDER_EXECUTED
} OrderResult;

typedef enum {
    CMD_NIL,
    CMD_WAIT,

    CMD_UP,
    CMD_DOWN,
    CMD_LEFT,
    CMD_RIGHT,

    CMD_HEALTH,
    CMD_DEINIT,
} CommandId;

typedef struct {
    CommandId id;
    int amount;
} Command;

typedef RenderData (*EntityRenderFn)(struct Entity);
typedef Command (*EntityUpdateFn)(struct Entity, Backend);
typedef OrderResult (*EntityOrderFn)(struct Entity, Allocator, Command);

typedef struct Entity {
    void * ctx;
    EntityRenderFn render;
    EntityUpdateFn update;
    EntityOrderFn order;
} Entity;


/*entity creation function*/
Entity immovable_create(Allocator a, char ch, uint8_t color);

#endif /*ENTITY_H*/


#ifdef ENTITY_IMPLEMENTATION
typedef struct {
    RenderData render_data;
} ImmovableContext;

RenderData immovable_render(struct Entity self) {
    ImmovableContext* ctx = self.ctx;
    return ctx->render_data;
}

Command immovable_update(struct Entity self, Backend b) {
    Command result = {0};
    (void)self;
    (void)b;

    result.id = CMD_NIL;
    return result;
}

OrderResult immovable_order(struct Entity self, Allocator a, Command cmd) {
    if(cmd.id == CMD_DEINIT) {
        a.free(a, self.ctx);
        return ORDER_EXECUTED;
    }
    return ORDER_IGNORED;
}

Entity immovable_create(Allocator a, char ch, uint8_t color) {
    Entity self = {0};
    ImmovableContext * ctx = a.alloc(a, sizeof(ImmovableContext));
    assert(ctx);

    ctx->render_data.ch = ch;
    ctx->render_data.color = color;
    
    self.ctx = ctx;

    self.render = immovable_render;
    self.update = immovable_update;
    self.order =  immovable_order;

    return self;
}



#if 0
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t health;
    Backend b;
} PlayerContext;

RenderEvent player_render(struct Entity self) {
    PlayerContext * ctx = self.ctx;
    RenderEvent event = {0};
    event.ch = '@';
    event.x = ctx.x;
    event.y = ctx.y;
    event.color = 25;
    return event;
}

Command player_update(struct Entity self) {
    PlayerContext * ctx = self.ctx;
    Command result = {0};
    (void)self;
    (void)b;

    result.id = CMD_WAIT;
    return result;
}

void wall_order(struct Entity self, Command cmd) {
    if(cmd.id == CMD_DEINIT) {
        self.a.free(self.a, self.ctx);
    }
}
RenderEvent player

#endif

#endif /*ENTITY_IMPLEMENTATION*/
