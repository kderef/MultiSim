#include "raylib.h"
#include "const.h"

#include <stdio.h>
#include <stdint.h>

#include "ui/selector.c"
#include "ui/windowicon.c"

int main(void) {
    // seed rand
    SetRandomSeed(time(NULL));

    InitAudioDevice();

#ifndef DEBUG
    SetTraceLogLevel(LOG_ERROR);
#endif

    // raylib initialization
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_W, WINDOW_H, selected_get_window_title(Selected_None));
    // SetTargetFPS(300);
    SetExitKey(0);
    SetWindowMinSize(WINDOW_W, WINDOW_H);

    // load the default font (bundled IN exe)
    load_default_font();

#ifndef __APPLE__
    load_window_icon();
    SetWindowIcon(window_icon);
#endif

    // the selector manages all the games and renders the title screen
    Selector selector = selector_new();

    while (!WindowShouldClose()) {
        selector_update(&selector);
    }

    CloseWindow();

#ifndef __APPLE__
    unload_window_icon();
#endif

    unload_default_font();

    return 0;
}