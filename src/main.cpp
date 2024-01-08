#include "raylib.h"
#include "const.hpp"

#include <stdio.h>
#include <stdint.h>

#include "ui/selector.cpp"
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

    // initialization of bundled binary files
    assets_load();
    load_window_icon();
    SetWindowIcon(window_icon);

    // the selector manages all the games and renders the title screen
    Selector selector;

    while (!WindowShouldClose()) {
        selector.update();
    }

    CloseWindow();

    unload_window_icon();
    assets_unload();

    return 0;
}