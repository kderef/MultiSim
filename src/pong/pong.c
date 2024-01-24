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
    p.ball.velocity = BALL_DEFAULT_NEG_VELOCITY; //BALL_DEFAULT_VELOCITY;
    p.score = (Score){ 0, 0 };

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


void pong_reset_scored(Pong* p, bool left_scored) {
    static float paddle_y;

    paddle_y = (p->window_size.y / 2) - (PADDLE_HEIGHT / 2);
    p->paddle_right.y = paddle_y;
    p->paddle_left.y = paddle_y;
    p->ball.pos = vec2(
        left_scored ?
        PADDLE_PADDING * 2 + PADDLE_WIDTH :
        p->window_size.x - PADDLE_PADDING * 2 - PADDLE_WIDTH,
        p->window_size.y / 2
    );

    p->ball.velocity.x = left_scored ? BALL_DEFAULT_V : -BALL_DEFAULT_V;
    p->ball.velocity.y = GetRandomValue(0, 1) ? BALL_DEFAULT_V : -BALL_DEFAULT_V;

    p->countdown_passed = 0.0f;

    if (left_scored) p->score.left++;
    else p->score.right++;
}

void pong_reset(Pong* p) {
    static float paddle_y;

    bool left = GetRandomValue(0, 1);

    paddle_y = (p->window_size.y / 2) - (PADDLE_HEIGHT / 2);
    p->paddle_right.y = paddle_y;
    p->paddle_left.y = paddle_y;
    p->ball.pos = vec2(
        left ?
        PADDLE_PADDING * 2 + PADDLE_WIDTH :
        p->window_size.x - PADDLE_PADDING * 2 - PADDLE_WIDTH,
        p->window_size.y / 2
    );

    p->ball.velocity.x = left ? BALL_DEFAULT_V : -BALL_DEFAULT_V;
    p->ball.velocity.y = GetRandomValue(0, 1) ? BALL_DEFAULT_V : -BALL_DEFAULT_V;

    p->countdown_passed = 0.0f;
}

static inline void pong_draw_help(Pong* p) {
    BeginDrawing();
    ClearBackground(BLACK);
    // TODO

    GuiDrawRectangle(rect(20, 20, global_state.screen_w - 40, global_state.screen_h - 40), 1, GRAY, color(20, 20, 20));
    DrawTextD("Pong Help", global_state.screen_w / 2 - 60, 40, FONT_L, GOLD);

    GuiDrawText("Left Paddle controls:", rect(50, 150, 200, 20), TEXT_ALIGN_LEFT, GOLD);
    GuiDrawText("W: up", rect(50, 170, 200, 20), TEXT_ALIGN_LEFT, WHITE);
    GuiDrawText("A: down", rect(50, 190, 200, 20), TEXT_ALIGN_LEFT, WHITE);

    GuiDrawText("Right paddle controls:", rect(50, 230, 200, 20), TEXT_ALIGN_LEFT, GOLD);
    GuiDrawText("W: up", rect(50, 250, 200, 20), TEXT_ALIGN_LEFT, WHITE);
    GuiDrawText("A: down", rect(50, 270, 200, 20), TEXT_ALIGN_LEFT, WHITE);

    const size_t controls_right = global_state.screen_w - 270;
    GuiDrawText("Global controls:", rect(controls_right, 150, 200, 20), TEXT_ALIGN_LEFT, GOLD);
    GuiDrawText("R      : reset game", rect(controls_right, 170, 200, 20), TEXT_ALIGN_LEFT, WHITE);
    GuiDrawText("Space: pause/unpause game", rect(controls_right, 190, 200, 20), TEXT_ALIGN_LEFT, WHITE);

    if (GuiButton(rect(global_state.screen_w / 2 - 100, global_state.screen_h - 100, 200, 50), "Exit help")) {
        p->state = GameState_Running;
    }
}

static inline void pong_draw(Pong* p) {
    static float line_x, score_y, passed_time = 0.0f;
    static bool draw_help_btn = false;

    BeginDrawing();
    ClearBackground(DARKGRAY);

    line_x = p->window_size.x / 2.0f;
    score_y = p->window_size.y / 2.0f - 25.0f;

    passed_time += GetFrameTime();

    if (global_state.mouse_delta.x != 0.0f || global_state.mouse_delta.y != 0.0f) {
        draw_help_btn = true;
    }

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

    if (draw_help_btn && passed_time < 1.5f) {
        if (GuiButton(
            rect(global_state.screen_w - ICON_SIZE - 2, 2, ICON_SIZE, ICON_SIZE),
            "#193#")
            ) {
            p->state = GameState_Help;
        }
    }
    else {
        passed_time = 0.0f;
        draw_help_btn = false;
    }
}

// later defined
void pong_handle_collision(Pong*);

SelectedGame pong_update(Pong* p) {
    static float fixed_paddle_speed;

    p->window_size = vec2(GetScreenWidth(), GetScreenHeight());
    p->paddle_right.x = p->window_size.x - PADDLE_WIDTH - PADDLE_PADDING;

    p->dt = GetFrameTime();

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (p->state == GameState_Help) {
            p->state = GameState_Paused;
            return Selected_PONG;
        }
        return Selected_None;
    }

    if (p->state == GameState_Help) {
        pong_draw_help(p);
        return Selected_PONG;
    }

    if (IsKeyPressed(KEY_R)) {
        p->score.left = 0;
        p->score.right = 0;
        p->state = GameState_Running;

        pong_reset(p);
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
            (p->countdown_passed >= 3.0f) ?
            -1.0f : p->countdown_passed + p->dt;
    }
    else {
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
        y = p->window_size.y / 2.0f - 70.0f;

        if (p->countdown_passed <= 1.0f)
            DrawTextD("3", x, y, size, GREEN);
        else if (p->countdown_passed <= 2.0f)
            DrawTextD("2", x, y, size, YELLOW);
        else DrawTextD("1", x + 12, y, size, RED);
    }
    else {
        p->ball.pos.x += p->ball.velocity.x * p->dt;
        p->ball.pos.y += p->ball.velocity.y * p->dt;

        pong_handle_collision(p);
        pong_draw(p);
    }
    return Selected_PONG;
}

void pong_handle_collision(Pong* p) {
    // top of screen / bottom of screen
    if (p->ball.pos.y <= p->ball.radius || p->ball.pos.y >= global_state.screen_h - p->ball.radius) {
        p->ball.velocity.y *= -1.1;
        PlaySound(p->hit_sound);
    }

    // right scored
    if (p->ball.pos.x <= 0.0) {
        pong_reset_scored(p, false);
        PlaySound(p->score_sound);
    }

    // left scored
    else if (p->ball.pos.x >= global_state.screen_w) {
        pong_reset_scored(p, true);
        PlaySound(p->score_sound);
    }

    // left or right paddle
    Paddle paddle = (p->ball.velocity.x < 0.0) ? p->paddle_left : p->paddle_right;

    // collision
    if (CheckCollisionCircleRec(
        p->ball.pos, p->ball.radius, paddle
    )) {
        if (p->ball.pos.y < paddle.y || p->ball.pos.y > paddle.y + paddle.height) {
            TraceLog(LOG_WARNING, "ball = (%f, %f), paddle = (%f, %f)", p->ball.pos.x, p->ball.pos.y, paddle.x, paddle.y);
            p->ball.velocity.y *= -BALL_VY_INCREASE;
        }
        p->ball.velocity.x *= -BALL_VX_INCREASE;
        PlaySound(p->hit_sound);
    }
}

#endif