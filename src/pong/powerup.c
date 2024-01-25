#ifndef POWERUP_C
#define POWERUP_C

#include "pong.c"
#include "../gamestate.h"    

#include "raylib.h"

#define PONG_POWERUP_SIZE 120.0f
#define PONG_POWERUP_SIZEV (Vector2){PONG_POWERUP_SIZE, PONG_POWERUP_SIZE}
#define PONG_POWERUP_MAX 128

typedef enum {
    POW_BALL_REVERSE = 0,
    POW_BALL_SPEED_UP,
    POW_PADDLE_ENLARGE,
    POW_PADDLE_OTHER_SHRINK,
    POW_BALL_SPAWN
} PongPowerup_t;

typedef struct {
    PongPowerup_t type;
    Vector2 pos;
} PongPowerup;

static PongPowerup PONG_POWERUPS[PONG_POWERUP_MAX];
static int pong_powerup_count = 0;

/// spawn random powerup
void pong_spawn_powerup(Pong* p) {
    const int powerup_pad = 120;
    PongPowerup_t selected = GetRandomValue(0, POW_BALL_SPAWN);
    Vector2 location = {
        GetRandomValue(powerup_pad, global_state.screen_w - PONG_POWERUP_SIZE - powerup_pad),
        global_state.screen_h
    };

    pong_powerup_count++;
}

void pong_render_powerups(Pong* p) {
    // TODO
}

#endif