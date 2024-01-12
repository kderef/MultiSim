#ifndef PONG_C_
#define PONG_C_

#include "raylib.h"
#include "raymath.h"
#include "../const.hpp"
#include "../gamestate.hpp"
#include "../assets.h"

#include "paddle.hpp"
#include "ball.hpp"

INCBIN(pong_hit_sound, "assets/beep.wav");

struct Score {
    uint32_t left;
    uint32_t right;
};

struct Pong {
private:
    float paddle_y;
    float abs_vx;

public:
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
    float line_x, score_y;

    Pong() {
        window_size = vec2(GetScreenWidth(), GetScreenHeight());
        Vector2 center;
        center.x = window_size.x / 2;
        center.y = window_size.y / 2;

        float paddle_y = center.y - PADDLE_HEIGHT / 2;

        ball.radius = BALL_RADIUS;
        ball.pos = center;
        ball.velocity = BALL_DEFAULT_VELOCITY;
        score = (Score) {0, 0};

        paddle_left = rect(
            PADDLE_PADDING, paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT
        );
        paddle_right = rect(
            window_size.x - PADDLE_PADDING - PADDLE_WIDTH, paddle_y,
            PADDLE_WIDTH,
            PADDLE_HEIGHT
        );

        state = GameState_Help;
        countdown_passed = 0.0f;
        hit_sound = LoadSoundFromWave(
            LoadWaveFromMemory(
                ".wav", pong_hit_sound_data, pong_hit_sound_size
            )
        );
        score_sound = LoadSoundFromWave(
            LoadWaveFromMemory(
                ".wav", pong_hit_sound_data, pong_hit_sound_size
            )
        );
        SetSoundVolume(hit_sound, 0.3);
        SetSoundVolume(score_sound, 0.5);
        SetSoundPitch(score_sound, 1.6);
    }

    ~Pong() {
        UnloadSound(hit_sound);
        UnloadSound(score_sound);
    }

    void reset(bool left_scored) {
        paddle_y = (window_size.y / 2) - (PADDLE_HEIGHT / 2);

        paddle_right.y = paddle_y;
        paddle_left.y = paddle_y;
        ball.pos = vec2(
            left_scored?
                PADDLE_PADDING*2 + PADDLE_WIDTH :
                window_size.x - PADDLE_PADDING*2 - PADDLE_WIDTH,
            window_size.y / 2
        );

        abs_vx = fabsf(ball.velocity.x);

        ball.velocity.x = left_scored? abs_vx : -abs_vx;
        ball.velocity.y = GetRandomValue(0, 1)? 1.0f : -1.0f;

        countdown_passed = 0.0f;
    }

    void draw_help() {
        BeginDrawing();
        ClearBackground(BLACK);
        // TODO
    }

    void draw() {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawTextD("Pong is work in progress :P", window_size.x / 2 - 300, window_size.y / 2 - 50, 50.0, GOLD);
        return;

        line_x = window_size.x / 2.0f;
        score_y = window_size.y / 2.0f - 25.0f;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawLine(line_x, window_size.y, line_x, 0.0f, DARKGRAY);
        
        g_sprintf("%u", score.left);
        DrawTextD(
            global_text_buf, line_x - 75.0,
            score_y,
            50.0, GRAY
        );
        g_sprintf("%u", score.right);
        DrawTextD(
            global_text_buf, line_x + 50.0,
            score_y,
            50.0, GRAY
        );

        DrawCircleV(ball.pos, ball.radius, WHITE);

        DrawRectangleRec(
            paddle_left,
            RAYWHITE
        );
        DrawRectangleRec(
            paddle_right,
            RAYWHITE
        );
    }

    SelectedGame update() {
        window_size = vec2(GetScreenWidth(), GetScreenHeight());
        paddle_right.x = window_size.x - PADDLE_WIDTH - PADDLE_PADDING;

        this->draw();

        dt = GetFrameTime();

        if (IsKeyPressed(KEY_ESCAPE)) {
            return Selected_None;
        }
        return Selected_PONG; // NOTE remove later :p

        if (IsKeyPressed(KEY_H)) {
            state = (state == GameState_Help)? GameState_Running : GameState_Help;
            this->draw();
            return Selected_PONG;
        }

        if (state == GameState_Help) {
            this->draw_help();
            return Selected_PONG;
        }

        if (IsKeyPressed(KEY_R)) {
            score.left = 0;
            score.right = 0;
            state = GameState_Running;

            this->reset(GetRandomValue(0, 1));
        }
        if (IsKeyPressed(KEY_SPACE)) {
            state = (state == GameState_Running) ? GameState_Paused : GameState_Running;
            this->draw();
            return Selected_PONG;
        }

        if (state == GameState_Paused) {
            this->draw();
            DrawTextD(
                "PAUSED",
                3, window_size.y - 25,
                25.0, GREEN
            );
            return Selected_PONG;
        }

        if (countdown_passed >= 0.0f) {
            countdown_passed =
                (countdown_passed >= 3.0f)?
                -1.0f : countdown_passed + dt;
        } else {
            fixed_paddle_speed = PADDLE_SPEED * dt;
            if (IsKeyDown(KEY_W)) {
                paddle_left.y = Clamp(
                    paddle_left.y - fixed_paddle_speed,
                    0.0f,
                    window_size.y - PADDLE_HEIGHT
                );
            }
            if (IsKeyDown(KEY_S)) {
                paddle_left.y = Clamp(
                    paddle_left.y + fixed_paddle_speed,
                    0.0f,
                    window_size.y - PADDLE_HEIGHT
                );
            }
            if (IsKeyDown(KEY_UP)) {
                paddle_right.y = Clamp(
                    paddle_right.y - fixed_paddle_speed,
                    0.0f,
                    window_size.y - PADDLE_HEIGHT
                );
            }
            if (IsKeyDown(KEY_DOWN)) {
                paddle_right.y = Clamp(
                    paddle_right.y + fixed_paddle_speed,
                    0.0f,
                    window_size.y - PADDLE_HEIGHT
                );
            }

        }

        if (countdown_passed < 0.0f) {
            ball.pos.x += ball.velocity.x * dt;
            ball.pos.y += ball.velocity.y * dt;

            this->handle_collision();
            this->draw();
        } else {
            this->draw();

            static float x, y, size = 125.0;
            x = window_size.x / 2.0f - 30.0f;
            y = window_size.y / 2.0f- 70.0f;

            if (countdown_passed <= 1.0f)
                DrawTextD(
                    "3", x, y, size, GREEN
                );
            else if (countdown_passed <= 2.0f)
                DrawTextD(
                    "2", x, y, size, YELLOW
                );
            else
                DrawTextD(
                    "1", x + 12, y, size, RED
                );
        }
        return Selected_PONG;
    }

    void handle_collision() {
        // TODO
    }
};

#endif