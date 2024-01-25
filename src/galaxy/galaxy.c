#ifndef GALAXY_C
#define GALAXY_C

#include "raylib.h"
#include "galaxy.h"
#include <stdlib.h>

static bool mouse_disabled = false;

Galaxy* galaxy_alloc(void) {
    Galaxy* g = (Galaxy*) malloc(sizeof(Galaxy));
    g->camera = (Camera3D){
            .fovy = 90.0,
            .projection = CAMERA_PERSPECTIVE,
            .up = vec3(0, 100, 0),
            .position = vec3(0, GX_CAMERA_DEFAULT_Y, 0),
            .target = vec3(100, 10, 100)
    };

    g->velocity = VEC3_ZERO;
    g->dash_bar = 0.0f;

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

    BeginMode3D(g->camera);
    UpdateCamera(&(g->camera), CAMERA_FIRST_PERSON);
    DrawGrid(500, 2.0);
    EndMode3D();

    g_sprintf("pos: [%.2f, %.2f, %.2f]", g->camera.position.x, g->camera.position.y, g->camera.position.z);
    DrawTextD(global_text_buf, 0, 23, 30.0, RAYWHITE);

    static const int DASHBAR_HEIGHT = 20;
    static const int DASHBAR_WIDTH = 300;

    GuiProgressBar(rect(10, global_state.screen_h - 10 - DASHBAR_HEIGHT, DASHBAR_WIDTH, DASHBAR_HEIGHT),
        "", "DASH", &g->dash_bar, 0.0f, 1.0f
    );

    // TODO
}

SelectedGame galaxy_update(Galaxy* g) {
    int key = GetKeyPressed();
    float dt = GetFrameTime();

    if (global_state.left_mouse_down) {}

    g->dash_bar = max(g->dash_bar + 20.0f * dt, 0.0f);

    switch (key) {
    case KEY_ESCAPE: {
        mouse_disabled = false;
        EnableCursor();
        return Selected_None;
    } break;
    case KEY_F5: {
        global_state.show_fps = !(global_state.show_fps);
    } break;
    case KEY_LEFT_SHIFT: {
        if (g->dash_bar == 1.0f) {
            g->dash_bar = 0.0f;
            // TODO fix dashing
            g->camera.position.x += 20.0f;
        }
    } break;
    default: {}
    }

    galaxy_draw(g);

    return Selected_GALAXY;
}

void galaxy_free(Galaxy* g) {
    if (mouse_disabled) {
        mouse_disabled = false;
        DisableCursor();
    }
    free(g);
}

#endif