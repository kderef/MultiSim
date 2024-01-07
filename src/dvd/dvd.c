#ifndef DVD_C_
#define DVD_C_

#include "raylib.h"
#include "raymath.h"
#include "../ui/raygui_incl.h"
#include "../gamestate.c"
#include "../const.h"
#include "../ui/font.c"

//! All of the code for rendering and handling the DvD 'game'.

// dvd logo bytes
INCBIN(dvd_logo, "assets/DVD_logo.png");

typedef struct {
    GameState state;
    Image logo_png;
    Texture2D logo;
    Vector2 position;
    Vector2 velocity;
    Vector2 logo_size;
    bool inverted, rainbow;
    float passed_time;
} Dvd;

void dvd_logo_set_random_pos(Dvd* self) {
    self->position = vec2(
        GetRandomValue(0, global_screen_width - self->logo.width),
        GetRandomValue(0, global_screen_height - self->logo.height)
    );
}

Dvd dvd_new(void) {
    Dvd d;
    d.state = GameState_Help;
    d.logo_png = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);

    d.logo = LoadTextureFromImage(d.logo_png);

    d.logo_size = vec2(d.logo.width, d.logo.height);
    d.velocity = vec2(250, 250);
    d.inverted = true;
    d.rainbow = true;
    d.position = vec2(
        GetRandomValue(0, GetScreenWidth() - d.logo.width),
        GetRandomValue(0, GetScreenHeight() - d.logo.height)
    );

    return d;
}

void dvd_draw_help(Dvd* self) {
    static float spacing;
    spacing = 0.0f;

    BeginDrawing();
    ClearBackground(BLACK);

    GuiButton(
        rect(50, 50, 300, 300), "Hello World!"
    );

    DrawTextD("DvD controls", 3, 0, FONT_XL, GREEN);
    DrawTextD("H         - toggle help mode", 3, spacing += FONT_XL, FONT_M, RAYWHITE);
    DrawTextD("I          - invert the colors", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("R         - randomize the DvD logo position", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("C         - toggle rainbow colors", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("Space   - pause/unpause the DvD logo", 3, spacing += FONT_M, FONT_M, RAYWHITE);
}

SelectedGame dvd_update(Dvd* self) {
    static float dt;
    static int key;

    key = GetKeyPressed();
    switch (key) {
        case KEY_ESCAPE: {
            self->state = GameState_Paused;
            return Selected_None;   
        }
        case KEY_H: {
            self->state = (self->state == GameState_Help)? GameState_Paused : GameState_Help;
        } break;
        case KEY_I: {
            self->inverted = !(self->inverted);
        } break;
        case KEY_R: {
            dvd_logo_set_random_pos(self);
        } break;
        case KEY_C: {
            self->rainbow = !(self->rainbow);
        } break;
        case KEY_SPACE: {
            self->state = (self->state == GameState_Running)? GameState_Paused : GameState_Running;
        } break;
        case KEY_F5: {
            global_show_fps = !global_show_fps;
        } break;
        default:
    }
    
    dt = GetFrameTime();
    self->passed_time = (self->passed_time > 36.0f)? 0.0f : self->passed_time + dt;

    switch (self->state) {
        case GameState_Help: 
            dvd_draw_help(self);
            return Selected_DVD;
        case GameState_Running: goto update_dvd;
        case GameState_Paused: goto draw_dvd;
        default: {}
    }

update_dvd:
    self->position = Vector2Add(
        Vector2Multiply(
            (Vector2){dt, dt},
            self->velocity
        ),
        Vector2Clamp(
            self->position,
            (Vector2){0, 0},
            Vector2Subtract(
                vec2(global_screen_width, global_screen_height),
                self->logo_size
            )
        )
    );

    if (
        self->position.x + self->logo.width >= global_screen_width ||
        self->position.x <= 0
    ) self->velocity.x *= -1;
    
    if (self->position.y + self->logo.height >= global_screen_height ||
        self->position.y <= 0
    ) self->velocity.y *= -1;

draw_dvd:
    static Color tint;
    tint = self->rainbow?
        self->inverted?
            ColorFromHSV(self->passed_time * 10.0f, 1.0, 1.0) :
            ColorFromHSV(self->passed_time * 10.0, 0.6, 1.0)
        : self->inverted? WHITE : BLACK;

    BeginDrawing();
    ClearBackground((self->inverted)? BLACK : RAYWHITE);
    if (self->state == GameState_Paused) {
        DrawTextD("[SPACE TO UNPAUSE]", 3, global_screen_height - FONT_S, FONT_S, GREEN);
    }
    DrawTexture(
        self->logo,
        self->position.x,
        self->position.y,
        tint
    );

    return Selected_DVD;
}

// destructor
void dvd_deinit(Dvd* self) {
    UnloadImage(self->logo_png);
    UnloadTexture(self->logo);
}

#endif