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

    // the selector manages all the games and renders the title screen
    Selector selector = {0};

#ifndef DEBUG
    SetTraceLogLevel(LOG_ERROR);
#endif

    // raylib initialization
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_W, WINDOW_H, selected_get_window_title(selector.selected));
    // SetTargetFPS(300);
    SetExitKey(0);
    SetWindowMinSize(WINDOW_W, WINDOW_H);

    // initialization of bundled binary files
    load_default_font();
    load_window_icon();
    SetWindowIcon(window_icon);

    // this will create all the game structs
    selector_init(&selector);

    while (!WindowShouldClose()) {
        selector_update(&selector);
    }

    selector_deinit(&selector);
    CloseWindow();

    unload_window_icon();

    return 0;
}