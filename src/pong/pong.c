#ifndef PONG_C_
#define PONG_C_

#include "raylib.h"
#include "raymath.h"
#include "../const.h"
#include "../gamestate.h"

#include "paddle.h"
#include "ball.h"

#include <stdbool.h>

INCBIN(pong_hit_sound, "assets/beep.wav");

typedef struct Score {
    uint32_t left;
    uint32_t right;
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
    p.state = GameState_Running;
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

void pong_deinit(Pong* p) {
    UnloadSound(p->hit_sound);
    UnloadSound(p->score_sound);
}

void pong_reset(Pong* p, bool left_scored) {
    static float paddle_y, abs_vx;

    paddle_y = (p->window_size.y / 2) - (PADDLE_HEIGHT / 2);
    p->paddle_right.y = paddle_y;
    p->paddle_left.y = paddle_y;
    p->ball.pos = vec2(
        left_scored?
            PADDLE_PADDING*2 + PADDLE_WIDTH :
            p->window_size.x - PADDLE_PADDING*2 - PADDLE_WIDTH,
        p->window_size.y / 2
    );

    abs_vx = fabsf(p->ball.velocity.x);

    p->ball.velocity.x = left_scored? abs_vx : -abs_vx;
    p->ball.velocity.y = GetRandomValue(0, 1)? 1.0f : -1.0f;

    p->countdown_passed = 0.0f;
}

static inline void pong_draw_help(Pong* p) {
    (void)p; // FIXME

    BeginDrawing();
    ClearBackground(BLACK);
    // TODO
}

static inline void pong_draw(Pong* p) {
    static float line_x, score_y;
    
    BeginDrawing();
    ClearBackground(DARKGRAY);

    line_x = p->window_size.x / 2.0f;
    score_y = p->window_size.y / 2.0f - 25.0f;

    BeginDrawing();
    ClearBackground(BLACK);

    DrawLine(line_x, p->window_size.y, line_x, 0.0f, DARKGRAY);
    
    g_sprintf("%u", p->score.left);
    DrawTextD(
        global_text_buf, line_x - 75.0,
        score_y,
        50.0, GRAY
    );
    g_sprintf("%u", p->score.right);
    DrawTextD(
        global_text_buf, line_x + 50.0,
        score_y,
        50.0, GRAY
    );

    DrawCircleV(p->ball.pos, p->ball.radius, WHITE);

    DrawRectangleRec(
        p->paddle_left,
        RAYWHITE
    );
    DrawRectangleRec(
        p->paddle_right,
        RAYWHITE
    );
}

// later defined
inline void pong_handle_collision(Pong*);

SelectedGame pong_update(Pong* p) {
    static float fixed_paddle_speed;

    p->window_size = vec2(GetScreenWidth(), GetScreenHeight());
    p->paddle_right.x = p->window_size.x - PADDLE_WIDTH - PADDLE_PADDING;

    p->dt = GetFrameTime();

    if (IsKeyPressed(KEY_ESCAPE)) {
        return Selected_None;
    }
    return Selected_PONG; // NOTE remove later :p

    if (IsKeyPressed(KEY_H)) {
        p->state = (p->state == GameState_Help)? GameState_Running : GameState_Help;
        pong_draw(p);
        return Selected_PONG;
    }

    if (p->state == GameState_Help) {
        pong_draw_help(p);
        return Selected_PONG;
    }

    if (IsKeyPressed(KEY_R)) {
        p->score.left = 0;
        p->score.right = 0;
        p->state = GameState_Running;

        pong_reset(p, GetRandomValue(0, 1));
    }
    if (IsKeyPressed(KEY_SPACE)) {
        p->state = (p->state == GameState_Running) ? GameState_Paused : GameState_Running;
        pong_draw(p);
        return Selected_PONG;
    }

    if (p->state == GameState_Paused) {
        pong_draw(p);
        DrawTextD(
            "PAUSED",
            3, p->window_size.y - 25,
            25.0, GREEN
        );
        return Selected_PONG;
    }

    if (p->countdown_passed >= 0.0f) {
        p->countdown_passed =
            (p->countdown_passed >= 3.0f)?
            -1.0f : p->countdown_passed + p->dt;
    } else {
        fixed_paddle_speed = PADDLE_SPEED * p->dt;
        if (IsKeyDown(KEY_W)) {
            p->paddle_left.y = Clamp(
                p->paddle_left.y - fixed_paddle_speed,
                0.0f,
                p->window_size.y - PADDLE_HEIGHT
            );
        }
        if (IsKeyDown(KEY_S)) {
            p->paddle_left.y = Clamp(
                p->paddle_left.y + fixed_paddle_speed,
                0.0f,
                p->window_size.y - PADDLE_HEIGHT
            );
        }
        if (IsKeyDown(KEY_UP)) {
            p->paddle_right.y = Clamp(
                p->paddle_right.y - fixed_paddle_speed,
                0.0f,
                p->window_size.y - PADDLE_HEIGHT
            );
        }
        if (IsKeyDown(KEY_DOWN)) {
            p->paddle_right.y = Clamp(
                p->paddle_right.y + fixed_paddle_speed,
                0.0f,
                p->window_size.y - PADDLE_HEIGHT
            );
        }

    }
    if (p->countdown_passed >= 0.0f) {
        pong_draw(p);

        static float x, y, size = 125.0;
        x = p->window_size.x / 2.0f - 30.0f;
        y = p->window_size.y / 2.0f- 70.0f;

        if (p->countdown_passed <= 1.0f)
            DrawTextD("3", x, y, size, GREEN);
        else if (p->countdown_passed <= 2.0f)
            DrawTextD("2", x, y, size, YELLOW);
        else DrawTextD("1", x + 12, y, size, RED);
    } else {
        p->ball.pos.x += p->ball.velocity.x * p->dt;
        p->ball.pos.y += p->ball.velocity.y * p->dt;

        pong_handle_collision(p);
        pong_draw(p);
    }
    return Selected_PONG;
}

inline void pong_handle_collision(Pong* p) {
    (void)p; // FIXME
    // TODO
}


#endif