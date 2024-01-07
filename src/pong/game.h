#ifndef PONG_GAME_H
#define PONG_GAME_H

#include "../const.h"
#include "../gamestate.c"
#include "ball.h"
#include "paddle.h"
#include "raylib.h"

INCBIN(pong_hit_sound, "assets/beep.wav");

typedef struct {
    uint32_t left;
    uint32_t right;
} Score;

typedef struct {
    Paddle paddle_left;
    Paddle paddle_right;
    Ball ball;
    GameState state;
    float countdown_passed;
    Sound hit_sound;
    Sound score_sound;
    Score score;
    Vector2 window_size;
} Pong;

Pong pong_new() {
    Pong p;
    p.window_size = vec2(GetScreenWidth(), GetScreenHeight());
    Vector2 center;
    center.x = p.window_size.x / 2;
    center.y = p.window_size.y / 2;

    float paddle_y = center.y - PADDLE_HEIGHT / 2;

    p.ball.radius = BALL_RADIUS;
    p.ball.pos = center;
    p.ball.velocity = BALL_DEFAULT_VELOCITY;
    p.score = (Score) {0, 0};

    p.paddle_left = rect(
        PADDLE_PADDING, paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT
    );
    p.paddle_right = rect(
        p.window_size.x - PADDLE_PADDING - PADDLE_WIDTH, paddle_y,
        PADDLE_WIDTH,
        PADDLE_HEIGHT
    );
    p.state = GameState_Help;
    p.countdown_passed = 0.0f;
    p.hit_sound = LoadSoundFromWave(
        LoadWaveFromMemory(
            ".wav", pong_hit_sound_data, pong_hit_sound_size
        )
    );
    p.score_sound = LoadSoundFromWave(
        LoadWaveFromMemory(
            ".wav", pong_hit_sound_data, pong_hit_sound_size
        )
    );
    SetSoundVolume(p.hit_sound, 0.3);
    SetSoundVolume(p.score_sound, 0.5);
    SetSoundPitch(p.score_sound, 1.6);

    return p;
}

#endif