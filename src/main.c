#define VERSION "2.0.5"

// reduce file size on windows
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#endif

#include "raylib.h"
#include "const.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "ui/selector.c"
#include "ui/windowicon.c"
#include "ui/splashtext.c"

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
    SetTargetFPS(
        GetMonitorRefreshRate(
            GetCurrentMonitor()
        ) * 2
    );

    // load the default font (bundled IN exe)
    load_default_font();

    // set the window icon
    load_window_icon();

    // load the random splash text
    load_random_splash_text();

    // the selector manages all the games and renders the title screen
    selector_init();

    while (!WindowShouldClose()) {
        selector_update();
    }

    // close the window and unload all assets (textures, images, etc.)
    CloseWindow();
    selector_deinit();
    unload_default_font();

    return 0;
}