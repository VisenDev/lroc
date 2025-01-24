#include <stdbool.h>
#include <assert.h>
#include <string.h>
#define ALLOCATOR_IMPLEMENTATION
#define USE_STDLIB 1
#include "pimbs/src/allocator.h"
#include "backend.h"
#include "pimbs/src/static_vec.h"


/*debug options*/

/*#define DEBUG_PATHFINDING*/

/*macros*/
#define entity_cap 4096
#define map_width 60
#define map_height 22
#define cache_size 8

#define is_point_on_edge(x, y, width, height) (x == 0 || y == 0 || x == width - 1 || y == height -1)
#define is_point_within_bounds(x, y, width, height) (x >= 0 && y >= 0 && x < width && y < height)
#define is_point_on_map(x, y) is_point_within_bounds(x, y, map_width, map_height)


/*backend*/
#if defined(BACKEND_NCURSES)
    #include "ncurses_backend.c"
#elif defined(BACKEND_ANSI)
    #include "ansi_backend.c"
#elif defined(BACKEND_DUMMY) 
    #include "dummy_backend.c"
#else
    #warning "No backend specified, defaulting to ANSI"
    #include "ansi_backend.c"
#endif /*BACKEND*/


/*forward declarations*/
struct Scene;
struct RunTime;
struct Entity;
static int valid_destination(struct Scene * const s, short x, short y);

typedef struct Id {
    short i;
} Id;
const Id null_id = {0};


typedef struct Point {
    short x;
    short y;
} Point;
const Point direction_offsets[4] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

short point_distance(Point a, Point b) {
    short result = 0; 
    result += abs(a.x - b.x);
    result += abs(a.y - b.y);
    return result;
}


#define max_path_len 128
typedef StaticVec(Point, max_path_len) Path;

typedef struct {
    Path path;
    Point start;
    Point finish;
} PathNode;


/*void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))*/
int pathnode_compare(const void * lhs, const void * rhs) {
    /*Point x = */
    const PathNode * a = lhs;
    const PathNode * b = rhs;
    const short a_distance = point_distance(svec_top(a->path), a->finish);
    const short b_distance = point_distance(svec_top(b->path), b->finish);

    assert(a->start.x == b->start.x);
    assert(a->start.y == b->start.y);
    assert(a->finish.x == b->finish.x);
    assert(a->finish.y == b->finish.y);

    return a_distance < b_distance;
}

typedef enum {
    PATH_DIRECT,
    PATH_BACKTRACK
} PathfindType;

Path pathfind_to(struct Scene * s, struct RunTime * r, PathfindType type, short x, short y, short destx, short desty) {
    Path empty_path = {0};
    const unsigned int max_iterations = 1024;
    unsigned int iterations = 0;
    StaticVec(PathNode, max_path_len) queue = {0};
    bool explored[map_width][map_height] = {0};

    /*add initial points*/
    Point start = {x, y};
    Point finish = {destx, desty};
    PathNode first_node = {empty_path, start, finish};
    svec_append(queue, first_node);
    (void)r;

    for(iterations = 0; iterations < max_iterations; ++iterations) {
        if(queue.len == 0) break;
        unsigned int dir = 0;
        const PathNode top = svec_pop(queue);
        const Point location = top.path.len == 0 ? start : svec_top(top.path);
        explored[location.x][location.y] = true;

        /* explore in all 4 directions from node*/
        for(dir = 0; dir < 4; ++dir) {

            /*add new point in direction to the path of a new pathnode*/
            const Point offset = direction_offsets[dir];
            const Point new_location = {location.x + offset.x, location.y + offset.y};
            Path new_path = top.path;
            svec_append(new_path, new_location);

            /*check if destination has been found*/
            if(new_location.x == destx && new_location.y == desty) {

                if(type == PATH_BACKTRACK) {
                    /*reverse the path*/
                    unsigned int i = 0;
                    for(i = 0; i < new_path.len / 2; ++i) {
                        Point tmp = new_path.items[i];
                        svec_set(new_path, new_path.len - 1 - i, tmp);
                        svec_set(new_path, i, svec_get(new_path, new_path.len - 1 -i));
                    }
                }
                return new_path;
            }

            if(valid_destination(s, new_location.x, new_location.y)
                    && explored[new_location.x][new_location.y] == false) {
                PathNode new_node = {new_path, top.start, top.finish};
                svec_append(queue, new_node);
            }
        }

        /*sort the queue for priority*/
        qsort(queue.items, queue.len, sizeof(PathNode), pathnode_compare);
    }



    
    return empty_path; 
}

/*Scene and entities*/


typedef enum {
    player,
    basic_melee,
    decoration,
} EntityType;

typedef struct Entity {
    EntityType type;

    short x;
    short y;
    char ch;
    short color;

    short health;
    bool block_movement;

    Path path;
} Entity;



typedef struct Scene {
    Id player_id;
    bool entity_used[entity_cap];
    Entity entity[entity_cap];
    StaticVec(Id, cache_size) entity_location_cache[map_width][map_height];
} Scene;

typedef struct RunTime{
    Allocator a;
    Backend b;    
    Action input;
} RunTime;

Id new_entity(Scene * s) {
    unsigned int i = 0;
    for(i = 0; i < entity_cap; ++i) {
        if(s->entity_used[i] == false) {
            const Id result = {i};
            s->entity_used[i] = true;
            return result;
        }
    }
    return null_id;
}

void delete_entity(Scene * s, Id id) {
    assert(id.i >= 0);
    assert(id.i < entity_cap);
    s->entity_used[id.i] = false;
}

Entity * get_entity(Scene * s, Id id) {
    assert(id.i != null_id.i);
    assert(id.i >= 0);
    assert(id.i < entity_cap);
    return &s->entity[id.i];
}

Entity * get_player(Scene * const s) {
    if(s->player_id.i) {
        return get_entity(s, s->player_id);
    } else {
        return NULL;
    }
}

static Entity * get_new_entity(Scene * s) {
    Id id = new_entity(s);
    return get_entity(s, id);
} 


/*pathfinding*/
static int valid_destination(struct Scene * const s, short x, short y) {
    unsigned long i = 0;
    int movement_blocked = 0;
    for(i = 0; i < s->entity_location_cache[x][y].len; ++i) {
        Id id = svec_get(s->entity_location_cache[x][y], i);
        const struct Entity * entity = get_entity(s, id);
        if(entity->block_movement){
            movement_blocked = 1;
        }
    }
    return is_point_on_map(x, y) && (movement_blocked == false);
}

void move_entity(Scene * s, Entity * entity, Action direction) {
    switch(direction) {
        case ACTION_UP: 
            if(valid_destination(s, entity->x, entity->y - 1)) {
                entity->y -= 1;
            }
            break;
        case ACTION_DOWN: 
            if(valid_destination(s, entity->x, entity->y + 1)) {
                entity->y += 1;
            }
            break;
        case ACTION_LEFT: 
            if(valid_destination(s, entity->x - 1, entity->y)) {
                entity->x -= 1;
            }
            break;
        case ACTION_RIGHT: 
            if(valid_destination(s, entity->x + 1, entity->y)) {
                entity->x += 1;
            }
            break;
        default:
            /*this function should only be called with a movement action*/
            abort();
            break;
    }
}


/*specific*/
void update_player(Scene * s, RunTime * r, Id id) {
    Entity * entity = get_entity(s, id);
    const Action action = r->input;
    s->player_id = id;
    assert(entity->type == player);
    switch(action) {
        case ACTION_UP: 
        case ACTION_DOWN: 
        case ACTION_LEFT: 
        case ACTION_RIGHT: 
            move_entity(s, entity, action);
            break;
        default:
            break;
    }
}


void update_basic_melee(Scene * s, RunTime * r, Id id) {
    Entity * entity = get_entity(s, id);
    Entity * player = NULL;
    if((player = get_player(s))) {
        if(entity->path.len < 5) {
            entity->path = pathfind_to(s, r, PATH_DIRECT, entity->x, entity->y, player->x, player->y);
        }
        if(entity->path.len != 0) {
            Point top = svec_get(entity->path, 0);

            svec_remove(entity->path, 0);

            if(top.x != entity->x) {
                move_entity(s, entity, top.x < entity->x ? ACTION_LEFT : ACTION_RIGHT);
            } else if(top.y != entity->y) {
                move_entity(s, entity, top.y < entity->y ? ACTION_UP: ACTION_DOWN);
            } else {
                /*this means x and y are equal*/
            }
        }
    }
}


/*generic*/
void render(Scene * s, RunTime * r, Id id) {
    const Entity * entity = get_entity(s, id);
    RenderEvent event = {entity->x, entity->y, entity->ch, entity->color};
    r->b.render(r->b, event);

#ifdef DEBUG_PATHFINDING
    {
        unsigned int i = 0;
        for(i = 0; i < entity->path.len; ++i) {
            const Point path_point = svec_get(entity->path, i);
            event.x = path_point.x;
            event.y = path_point.y;
            event.ch = (char)i + '1';
            event.color = 2;
            r->b.render(r->b, event);
        }
    }
#endif /*DEBUG_PATHFINDING*/
}

void update(Scene * s, RunTime * r, Id id) {
    const Entity * entity = get_entity(s, id);
    switch(entity->type) {
        case player: update_player(s, r, id); break;
        case basic_melee: {} update_basic_melee(s, r, id); break;
        case decoration: {} break;
    }
}

/*creation functions*/
Entity * create_decoration(Scene * self, short x, short y, char ch, short color) {
    Entity * entity = get_new_entity(self);
    entity->x = x;
    entity->y = y;
    entity->ch = ch;
    entity->color = color;
    entity->type = decoration;
    return entity;
}

Scene create_scene(void) {
    short x = 0;
    short y = 0;
    Scene result = {0};

    /*entity the null entity isn't used for real entities*/
    result.entity_used[0] = true;

    for(x = 0; x < map_width; ++x) {
        for(y = 0; y < map_height; ++y) {
            if(is_point_on_edge(x, y, map_width, map_height)){
                Entity * entity = create_decoration(&result, x, y, '#', 42);
                entity->block_movement = true;
            } else {
                if(rand() % 5 == 0) { 
                    create_decoration(&result, x, y, ',', 42);
                } else if(rand() % 20 == 0) { 
                    Entity * entity = create_decoration(&result, x, y, '#', 160);
                    entity->block_movement = true;
                } else {
                    create_decoration(&result, x, y, '.', 3);
                }
            }
        }
    }

    Entity * entity = create_decoration(&result, 5, 5, '@', 200);
    entity->type = player;

    /*entity = create_decoration(&result, 9, 9, 'r', 3);
    entity->type = basic_melee;*/
    entity = create_decoration(&result, 11, 11, 'r', 3);
    entity->type = basic_melee;

    return result;
}

/* entry point */
int main(void) {
    Allocator a = libc_allocator();
    Backend b = backend_init(a);
    RunTime r = {a, b, ACTION_NIL};
    Scene s = create_scene();

    while(r.input != ACTION_QUIT) {
        unsigned int i = 0;

        /*update*/
        for(i = 1; i < entity_cap; ++i) {
            const Id id = {i};
            if(s.entity_used[i]) {
                update(&s, &r, id);
            }
        }

        /*cache positions*/
        memset(&s.entity_location_cache, 0, sizeof(s.entity_location_cache));
        
        for(i = 1; i < entity_cap; ++i) {
            const Id id = {i};
            if(s.entity_used[i]) {
                Entity * entity = get_entity(&s, id);
                short x = entity->x;
                short y = entity->y;
                svec_append(s.entity_location_cache[x][y], id);
            }
        }

        /*render*/
        r.b.begin_rendering(r.b);
        for(i = 1; i < entity_cap; ++i) {
            const Id id = {i};
            if(s.entity_used[i]) {
                render(&s, &r, id);
            }
        }
        r.b.finish_rendering(r.b);

        r.input = r.b.input(r.b);
    }
    r.b.deinit(r.b);

    return 0;
}
