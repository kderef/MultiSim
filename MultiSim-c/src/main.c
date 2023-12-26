#include "raylib.h"
#include "const.h"

#include <stdio.h>
#include <stdint.h>

#include "selector.c"

int main(void) {
    // load the default font
    load_default_font();

    // initialize game selector
    Selector selector = {0};

#ifndef DEBUG
    SetTraceLogLevel(LOG_ERROR);
#endif

    // raylib initialization
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GOL_WINDOW_W, GOL_WINDOW_H, selected_get_window_title(selector.selected));
    SetTargetFPS(200);
    SetExitKey(0);

    selector_init(&selector);

    while (!WindowShouldClose()) {
        selector_update(&selector);
    }

    selector_deinit(&selector);
    CloseWindow();

    return 0;
}