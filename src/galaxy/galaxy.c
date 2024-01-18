#ifndef GALAXY_C
#define GALAXY_C

#include "raylib.h"
#include "rcamera.h"
#include "galaxy.h"
#include <stdlib.h>

static bool mouse_disabled = false;

Galaxy galaxy_new(void) {
    return (Galaxy) {
        .camera = (Camera3D) {
            .fovy = 90.0,
            .projection = CAMERA_PERSPECTIVE,
            .up = vec3(0, 100, 0),
            .position = vec3(0, GX_CAMERA_DEFAULT_Y, 0),
            .target = vec3(100, 10, 100)
        },
        .velocity = vec3(0, 0, 0)
    };
}

static inline
void galaxy_draw(Galaxy* g) {
    if (!mouse_disabled) {
        mouse_disabled = true;
        DisableCursor();
    }

    BeginDrawing();
    ClearBackground(BLACK);

    g_sprintf("pos: [%.2f, %.2f, %.2f]", g->camera.position.x, g->camera.position.y, g->camera.position.z);
    DrawTextD(global_text_buf, 0, 23, 30.0, RAYWHITE);

    BeginMode3D(g->camera);
        UpdateCamera(&(g->camera), CAMERA_FIRST_PERSON);
        DrawGrid(500, 2.0);
        DrawCube(vec3(100, 10, 100), 100.0, 100.0, 100.0, RED);

    EndMode3D();
    
    // TODO
}

SelectedGame galaxy_update(Galaxy* g) {
    int key = GetKeyPressed();

    switch (key) {
        case KEY_ESCAPE: {
            mouse_disabled = false;
            EnableCursor();
            return Selected_None;
        } break;
        case KEY_F5: {
            global_state.show_fps = !(global_state.show_fps);
        } break;
        default: {}
    }

    galaxy_draw(g);

    return Selected_GALAXY;
}

void galaxy_deinit(Galaxy* g) {
    if (mouse_disabled) {
        mouse_disabled = false;
        DisableCursor();
    }
}

#endif