#ifndef DVD_C_
#define DVD_C_

#include "raylib.h"
#include "raymath.h"
#include "../ui/raygui_incl.h"
#include "../gamestate.h"
#include "../const.h"
#include "../ui/font.c"

//! All of the code for rendering and handling the DvD 'game'.

INCBIN(dvd_logo, "assets/DVD_logo.png");

typedef struct Dvd {
    GameState state;
    Vector2 position;
    Vector2 velocity;
    Texture2D logo;
    Vector2 logo_size;
    bool inverted, rainbow;
    float passed_time;
    int key;
} Dvd;

Dvd* dvd_alloc() {
    Dvd* d = calloc(1, sizeof(Dvd));
    d->state = GameState_Help;

    Image logo_png = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);
    d->logo = LoadTextureFromImage(logo_png);
    UnloadImage(logo_png);

    d->logo_size = vec2(d->logo.width, d->logo.height);
    d->velocity  = vec2(250, 250);
    d->inverted  = true;
    d->rainbow   = true;
    d->position  = vec2(
        GetRandomValue(0, GetScreenWidth()  - d->logo.width),
        GetRandomValue(0, GetScreenHeight() - d->logo.height)
    );
    return d;
}

void dvd_free(Dvd* d) {
    UnloadTexture(d->logo);
    free(d);
}

static inline void dvd_set_random_pos(Dvd* d) {
    d->position = vec2(
        GetRandomValue(0, global_state.screen_w - d->logo.width),
        GetRandomValue(0, global_state.screen_h - d->logo.height)
    );
}

static inline void dvd_draw_help() {
    static float spacing;
    spacing = FONT_M;
        
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextD("DvD controls", 3, FONT_M, FONT_XL, GREEN);
    DrawTextD("H         - toggle help mode", 3, spacing += FONT_XL, FONT_M, RAYWHITE);
    DrawTextD("I          - invert the colors", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("R         - randomize the DvD logo position", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("C         - toggle rainbow colors", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("Space   - pause/unpause the DvD logo", 3, spacing += FONT_M, FONT_M, RAYWHITE);
}

SelectedGame dvd_update(Dvd* d) {
    static float dt;

    static int key;
    key = GetKeyPressed();

    switch (key) {
        case KEY_ESCAPE: {
            d->state = GameState_Paused;
            return Selected_None;   
        }
        case KEY_H: {
            d->state = (d->state == GameState_Help)? GameState_Paused : GameState_Help;
        } break;
        case KEY_I: {
            d->inverted = !(d->inverted);
        } break;
        case KEY_R: {
            dvd_set_random_pos(d);
        } break;
        case KEY_C: {
            d->rainbow = !(d->rainbow);
        } break;
        case KEY_SPACE: {
            d->state = (d->state == GameState_Running)? GameState_Paused : GameState_Running;
        } break;
        case KEY_F5: {
            global_state.show_fps = !global_state.show_fps;
        } break;
        default: {}
    }
    
    dt = GetFrameTime();
    d->passed_time = (d->passed_time > 36.0f)? 0.0f : d->passed_time + dt;

    switch (d->state) {
        case GameState_Help: 
            dvd_draw_help();
            return Selected_DVD;
        case GameState_Running: goto update_dvd;
        case GameState_Paused: goto draw_dvd;
        default: {}
    }

update_dvd:
    d->position = Vector2Add(
        Vector2Multiply(
            (Vector2){dt, dt},
            d->velocity
        ),
        Vector2Clamp(
            d->position,
            (Vector2){0, 0},
            Vector2Subtract(
                vec2(global_state.screen_w, global_state.screen_h),
                d->logo_size
            )
        )
    );

    if (
        d->position.x + d->logo_size.x >= global_state.screen_w ||
        d->position.x <= 0
    ) d->velocity.x *= -1;
    
    if (d->position.y + d->logo_size.y >= global_state.screen_h ||
        d->position.y <= 0
    ) d->velocity.y *= -1;

    draw_dvd: {
        Color tint = d->rainbow?
            d->inverted?
                ColorFromHSV(d->passed_time * 10.0f, 1.0, 1.0) :
                ColorFromHSV(d->passed_time * 10.0, 0.6, 1.0)
            : d->inverted? WHITE : BLACK;

        BeginDrawing();
        ClearBackground((d->inverted)? BLACK : RAYWHITE);
        if (d->state == GameState_Paused) {
            DrawTextD("[SPACE TO UNPAUSE]", 3, global_state.screen_h - FONT_S, FONT_S, GREEN);
        }
        DrawTexture(
            d->logo,
            d->position.x,
            d->position.y,
            tint
        );
    }

    return Selected_DVD;
}

#endif