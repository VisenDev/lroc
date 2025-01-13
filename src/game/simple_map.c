#include "map.h"

typedef struct {
    const uint8_t w;
    const uint8_t h;
} SimpleContext;


void generate_simple_map(
        struct MapGenerator self,
        MapConfig cfg,
        Entity result[],
        uint32_t result_len) {



}

MapGenerator simple_map_generator(void) {
    static SimpleContext ctx = {80, 24};
    MapGenerator result = {0};
    result.ctx = &ctx;
    result.generate_map = generate_simple_map;

    return result;

}
