#ifndef DVD_C_
#define DVD_C_

#include "raylib.h"
#include "raymath.h"
#include "../gamestate.c"
#include "../const.h"
#include "../ui/font.c"

//! All of the code for rendering and handling the DvD 'game'.

// dvd logo bytes
INCBIN(dvd_logo, "assets/DVD_logo.png");

typedef struct {
    GameState state;
    Image logo_png;
    Image logo_png_inverted;
    Texture2D logo;
    Texture2D logo_inverted;
    Vector2 position;
    Vector2 velocity;
    Vector2 window_size;
    Vector2 logo_size;
    bool inverted;
} Dvd;

void dvd_logo_set_random_pos(Dvd* self) {
    self->position = (Vector2){
        GetRandomValue(0, self->window_size.x - self->logo.width),
        GetRandomValue(0, self->window_size.y - self->logo.height)
    };
}

Dvd dvd_new(void) {
    Dvd d;
    d.state = GameState_Help;
    d.logo_png = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);
    d.logo_png_inverted = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);
    ImageColorInvert(&(d.logo_png_inverted));

    d.logo = LoadTextureFromImage(d.logo_png);
    d.logo_inverted = LoadTextureFromImage(d.logo_png_inverted);

    d.window_size = vec2(GetScreenWidth(), GetScreenHeight());

    dvd_logo_set_random_pos(&d);
    d.logo_size = vec2(d.logo.width, d.logo.height);
    d.velocity = vec2(250, 250);

    d.inverted = false;

    return d;
}

void dvd_draw_help(Dvd* self) {
    static float spacing;
    spacing = 0.0f;

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextD("DvD controls", 3, 0, FONT_XL, GREEN);
    DrawTextD("H         - toggle help mode", 3, spacing += FONT_XL, FONT_M, RAYWHITE);
    DrawTextD("I          - invert the colors", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("R         - randomize the DvD logo position", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    DrawTextD("Space   - pause/unpause the DvD logo", 3, spacing += FONT_M, FONT_M, RAYWHITE);

    EndDrawing();
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
        case KEY_SPACE: {
            self->state = (self->state == GameState_Running)? GameState_Paused : GameState_Running;
        } break;
        default:
    }
    
    dt = GetFrameTime();

    self->window_size = (Vector2){GetScreenWidth(), GetScreenHeight()};

    switch (self->state) {
        case GameState_Help: 
            dvd_draw_help(self);
            return Selected_DVD;
        case GameState_Running: goto update_dvd;
        case GameState_Paused: goto draw_dvd;
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
                self->window_size,
                self->logo_size
            )
        )
    );

    if (
        self->position.x + self->logo.width >= self->window_size.x ||
        self->position.x <= 0
    ) self->velocity.x *= -1;
    
    if (self->position.y + self->logo.height >= self->window_size.y ||
        self->position.y <= 0
    ) self->velocity.y *= -1;


draw_dvd:
    BeginDrawing();
    ClearBackground((self->inverted)? BLACK : WHITE);
    if (self->state == GameState_Paused) {
        DrawTextD("[SPACE TO UNPAUSE]", 3, self->window_size.y - FONT_S, FONT_S, GREEN);
    }
    DrawTexture(
        (self->inverted)? self->logo_inverted : self->logo,
        self->position.x,
        self->position.y,
        WHITE
    );

    EndDrawing();

    return Selected_DVD;
}

// destructor
void dvd_deinit(Dvd* self) {
    UnloadImage(self->logo_png);
    UnloadImage(self->logo_png_inverted);
    UnloadTexture(self->logo);
    UnloadTexture(self->logo_inverted);
}

#endif