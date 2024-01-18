#ifndef GALAXY_C
#define GALAXY_C

#include "raylib.h"
#include "galaxy.h"
#include <stdlib.h>

static bool mouse_disabled = false;

Galaxy galaxy_new(void) {
    Galaxy g;

    g.sun = planet_sun();

    g.camera = (Camera3D) {
        .fovy = 90.0,
        .target = vec3(100, 100, 100),
        .projection = CAMERA_PERSPECTIVE,
        .up = vec3(0, 1, 0)
    };

    return g;
}

static inline
void galaxy_draw(Galaxy* g) {
    if (!mouse_disabled) {
        mouse_disabled = true;
        DisableCursor();
    }

    BeginDrawing();
    ClearBackground(BLACK);

    g_sprintf("Position: (%d, %d)", g->camera.position);
    DrawTextD(global_text_buf, 0, 3, 20.0, RAYWHITE);

    BeginMode3D(g->camera);
        UpdateCamera(&(g->camera), CAMERA_FREE);
        DrawGrid(500, 2.0);
        DrawSphere(
            g->sun.pos, g->sun.radius, g->sun.color
        );

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
    free(g->planets);
}



#endif