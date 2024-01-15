#include "raylib.h"
#include "const.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "ui/selector.c"
#include "ui/windowicon.c"

int main(void) {
    // seed random
    SetRandomSeed(time(NULL));

    // initialize audio backend
    InitAudioDevice();

    // if release mode, disable all logging messages except for LOG_ERROR
#ifndef DEBUG
    SetTraceLogLevel(LOG_ERROR);
#endif

    // raylib initialization
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_W, WINDOW_H, selected_get_window_title(Selected_None));
    SetExitKey(KEY_NULL);
    SetWindowMinSize(WINDOW_W, WINDOW_H);

    // load the default font (bundled IN exe)
    load_default_font();

    // set the window icon (not needed on MacOS)
#ifndef __APPLE__
    load_window_icon();
#endif

    // the selector manages all the games and renders the title screen
    selector_init();

    while (!WindowShouldClose()) {
        selector_update();
    }

    CloseWindow();

    unload_default_font();

    return 0;
}