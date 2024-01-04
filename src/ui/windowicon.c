#ifndef WINDOW_ICON_C_
#define WINDOW_ICON_C_

#include "raylib.h"
#include "../const.h"

static Image window_icon;

INCBIN(window_icon, "assets/icon/icon-big.png");

void load_window_icon(void) {
    window_icon = LoadImageFromMemory(
        ".png", window_icon_data, window_icon_size
    );
}

void unload_window_icon(void) {
    UnloadImage(window_icon);
}

#endif