#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"
#include "external/incbin.h"
#include "ui/font.c"

INCBIN(dvd_logo, "assets/DVD_logo.png");
INCBIN(bolus, "assets/bolus.png");
INCBIN(default_font, "assets/CnC-RedAlert.ttf");

static Texture dvd_logo_texture;
static Texture bolus_texture;
static Font font;

/// load all the images and other binary data included in the program
void assets_load(void) {
    Image dvd_logo = LoadImageFromMemory(".png", dvd_logo_data, dvd_logo_size);
    Image bolus = LoadImageFromMemory(".png", bolus_data, bolus_size);
    font = LoadFontFromMemory(
        ".ttf", default_font_data, default_font_size,
        100, NULL, 0
    );

    dvd_logo_texture = LoadTextureFromImage(dvd_logo);
    bolus_texture = LoadTextureFromImage(bolus);

    UnloadImage(dvd_logo);
    UnloadImage(bolus);
}

void assets_unload(void) {
    UnloadTexture(dvd_logo_texture);
    UnloadTexture(bolus_texture);
    UnloadFont(font);
}

#endif