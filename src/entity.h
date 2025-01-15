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

/*
typedef struct {
    CommandId id;
    int amount;
} Command;
*/

typedef RenderData (*EntityRenderFn)(struct Entity);
typedef Action (*EntityUpdateFn)(struct Entity);
typedef void (*EntityDestroyFn)(struct Entity, Allocator);
/*
typedef OrderResult (*EntityOrderFn)(struct Entity, Allocator, Command);
*/

typedef struct Entity {
    void * ctx;
    EntityRenderFn render;
    EntityUpdateFn update;
    EntityDestroyFn destroy;
} Entity;


/*entity creation function*/
Entity immovable_create(Allocator a, char ch, uint8_t color);
Entity player_create(Allocator a, Action*);

#endif /*ENTITY_H*/


#ifdef ENTITY_IMPLEMENTATION
typedef struct {
    RenderData render_data;
} ImmovableContext;

RenderData immovable_render(struct Entity self) {
    ImmovableContext* ctx = self.ctx;
    return ctx->render_data;
}

Action immovable_update(struct Entity self) {
    return ACTION_NIL;
}

void immovable_destroy(struct Entity self, Allocator a) {
    a.free(a, self.ctx); 
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

    return self;
}


/*Player*/
typedef struct {
    uint8_t health;
    Action * input_ptr;
} PlayerContext;

RenderData player_render(struct Entity self) {
    PlayerContext * ctx = self.ctx;
    RenderData data = {0};
    data.ch = '@';
    data.color = 25;
    return data;
}

Action player_update(struct Entity self) {
    PlayerContext * ctx = self.ctx;
    return *ctx->input_ptr;
}

void player_destroy(struct Entity self, Allocator a) {
    a.free(a, self.ctx);
}

Entity player_create(Allocator a, Action * input_ptr) {
    Entity self = {0};
    PlayerContext * ctx = a.alloc(a, sizeof(PlayerContext));
    assert(ctx);

    ctx->input_ptr = input_ptr;
    ctx->health = 10;
    
    self.ctx = ctx;
    self.render = player_render;
    self.update = player_update;
    self.destroy = player_destroy;

    return self;
}

#endif /*ENTITY_IMPLEMENTATION*/
