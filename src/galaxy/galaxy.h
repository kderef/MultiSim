#ifndef GALAXY_H
#define GALAXY_H

#include "../gamestate.h"
#include "raylib.h"
#include "player.h"

#define GX_CAMERA_DEFAULT_Y 5.0f

typedef struct Galaxy {
    Camera3D camera;
    Vector3 velocity;
} Galaxy;

Galaxy galaxy_new(void);
SelectedGame galaxy_update(Galaxy*);
void galaxy_deinit(Galaxy*);

#endif