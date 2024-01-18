#ifndef GALAXY_H
#define GALAXY_H

#include "../gamestate.h"
#include "raylib.h"
#include "planet.c"

typedef struct Galaxy {
    Camera3D camera;
    Planet planets[9];
    Planet sun;
    Planet earth;
    Planet moon;
} Galaxy;

Galaxy galaxy_new(void);
SelectedGame galaxy_update(Galaxy*);
void galaxy_deinit(Galaxy*);

#endif