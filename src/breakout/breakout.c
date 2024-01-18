#ifndef BREAKOUT_C
#define BREAKOUT_C

#include "breakout.h"
#include "../gamestate.h"

Breakout breakout_new(void) {
    return (Breakout) {
        .paddle = (Rectangle){GetScreenWidth(), GetScreenHeight()},
        .state = GameState_Paused,
    };
}

static inline void breakout_draw(Breakout* b) {
    static const Color bg = {29, 32, 33, 255};

    BeginDrawing();
    ClearBackground(bg);
}

SelectedGame breakout_update(Breakout* b) {
    int key = GetKeyPressed();

    switch (key) {
        case KEY_ESCAPE: return Selected_None;
        case KEY_F5: {
            global_state.show_fps = !(global_state.show_fps);
        } break;
        default: {}
    }

    breakout_draw(b);

    return Selected_BREAKOUT;
}

void breakout_deinit(Breakout* b) {
    (void)b;
}

#endif //BREAKOUT_C