#ifndef DVD_C_
#define DVD_C_

#include "raylib.h"
#include "../selectedgame.c"

typedef struct {
    Image logo_png;
    Texture2D logo;
} Dvd;

Dvd dvd_new(void) {
    Dvd d;
    d.logo_png = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);
    d.logo = LoadTextureFromImage(d.logo_png);

    return d;
}

SelectedGame dvd_update(Dvd* self) {

}

void dvd_deinit(Dvd* self) {
    UnloadImage(self->logo_png);
    UnloadTexture(self->logo);
}

#endif