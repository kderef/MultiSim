#ifndef PLANET_C
#define PLANET_C

#include "raylib.h"
#include "../const.h"

#define PLANET_COUNT 12

typedef struct Planet {
    float radius;
    Vector3 pos;
    Color color;
} Planet;

// Sun
const Planet planet_sun(void) {
    return (Planet){
        .radius = 10.0f,
        .pos = (Vector3){0.0f, 15.0f, 0.0f},
        .color = YELLOW
    };
}

#endif