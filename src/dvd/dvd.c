#ifndef DVD_C_
#define DVD_C_

#include "raylib.h"
#include "raymath.h"
#include "../gamestate.c"
#include "../const.h"

// dvd logo
INCBIN(dvd_logo, "assets/DVD_logo.png");

typedef enum {
    DVD_Running = 0,
    DVD_Paused,
    DVD_Help
} DvDGameState;

typedef struct {
    DvDGameState state;

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

Dvd dvd_new(void) {
    Dvd d;
    d.state = DVD_Help;
    d.logo_png = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);
    d.logo_png_inverted = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);
    ImageColorInvert(&(d.logo_png_inverted));

    d.logo = LoadTextureFromImage(d.logo_png);
    d.logo_inverted = LoadTextureFromImage(d.logo_png_inverted);

    d.position = (Vector2){
        GetRandomValue(0, GetScreenWidth() - d.logo.width),
        GetRandomValue(0, GetScreenHeight() - d.logo.height)
    };
    d.logo_size = (Vector2){d.logo.width, d.logo.height};
    d.velocity = (Vector2){250, 250};

    d.inverted = false;

    return d;
}

void dvd_draw_help(Dvd* self) {
    BeginDrawing();
    ClearBackground(BLACK);
}

SelectedGame dvd_update(Dvd* self) {
    static float dt;
    static int key;

    key = GetKeyPressed();
    switch (key) {
        case KEY_ESCAPE:
        return Selected_None;
        case KEY_H: {
            self->state = (self->state == DVD_Help)? DVD_Paused : DVD_Help;
        } break;
        case KEY_I: {
            self->inverted = !(self->inverted);
        }
        default:
    }
    
    dt = GetFrameTime();

    self->window_size = (Vector2){GetScreenWidth(), GetScreenHeight()};

    switch (self->state) {
        case DVD_Help: 
            dvd_draw_help(self);
            return Selected_DVD;
        case DVD_Running: goto update_dvd;
        case DVD_Paused: goto draw_dvd;
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