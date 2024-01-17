#ifndef SPLASHTEXT_H
#define SPLASHTEXT_H

#include "raylib.h"
#include "font.c"

#define SPLASH_TEXT_BASE 30.0

static const char* SPLASH_TEXT[] = {
    "All new!",
    "Bolus = life",
    "New battle pass",
    "Better than minecraft",
    "Hello, World!",
    "originally written in Rust!",
    "C for the win!",
    "C++ is terrible.",
    "Update TF2",
    "Crossplatform!",
    "C99 compliant!",
    "Posix > Windows",
    "Free to play!",
    "No python please.",
    "This text is randomly generated!",
    "No garbage collection!",
    "shoutout to John Conway"
};

static const char* splash_text;
static Vector2 splash_text_len;
static Vector2 splash_text_len_half;

void load_random_splash_text(void) {
    splash_text = SPLASH_TEXT[
        GetRandomValue(
            0, sizeof(SPLASH_TEXT) / sizeof(SPLASH_TEXT[0]) - 1
        )
    ];
    splash_text_len = MeasureTextEx(font, splash_text, SPLASH_TEXT_BASE, 1.0);
    splash_text_len_half = (Vector2) {
        .x = splash_text_len.x / 2,
        .y = splash_text_len.y / 2
    };
}

#endif