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

#endif //BREAKOUT_C