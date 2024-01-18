#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "raylib.h"
#include "../gamestate.h"

typedef struct Breakout {
    Vector2 ball_pos;
    Vector2 ball_velocity;
    Rectangle paddle;
    GameState state;
} Breakout;

Breakout breakout_new(void);
SelectedGame breakout_update(Breakout*);
static inline void breakout_draw(Breakout*);
void breakout_deinit(Breakout*);

#endif //BREAKOUT_H