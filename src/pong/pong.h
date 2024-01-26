#ifndef PONG_H
#define PONG_H

#include "paddle.h"
#include "ball.h"
#include "raylib.h"
#include "../gamestate.h"

typedef struct Score {
    uint32_t left;
    uint32_t right;
    uint64_t total;
} Score;

typedef struct Pong {
    Paddle paddle_left;
    Paddle paddle_right;
    Ball ball;
    GameState state;
    float countdown_passed;
    Sound hit_sound;
    Sound score_sound;
    Score score;
    Vector2 window_size;

    float dt;
    float fixed_paddle_speed;
} Pong;

#endif