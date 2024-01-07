#ifndef PONG_C_
#define PONG_C_

#include "raylib.h"
#include "raymath.h"
#include "../const.h"
#include "../gamestate.c"

#include "game.h"
#include "paddle.h"
#include "ball.h"
#include "collision.c"

void pong_reset(Pong* self, bool left_scored) {
    static float paddle_y, abs_vx;
    paddle_y = (self->window_size.y / 2) - (PADDLE_HEIGHT / 2);

    self->paddle_right.y = paddle_y;
    self->paddle_left.y = paddle_y;
    self->ball.pos = vec2(
        left_scored?
            PADDLE_PADDING*2 + PADDLE_WIDTH :
            self->window_size.x - PADDLE_PADDING*2 - PADDLE_WIDTH,
        self->window_size.y / 2
    );

    abs_vx = fabsf(self->ball.velocity.x);

    self->ball.velocity.x = left_scored? abs_vx : -abs_vx;
    self->ball.velocity.y = GetRandomValue(0, 1)? 1.0f : -1.0f;

    self->countdown_passed = 0.0f;
}

void pong_draw_help(Pong* self) {
    BeginDrawing();
    ClearBackground(BLACK);

    // TODO
}

void pong_draw(Pong* self) {
    static int line_x, score_y;
    static char buffer[128];

    BeginDrawing();
    ClearBackground(DARKGRAY);

    DrawTextD("Pong is work in progress :P", self->window_size.x / 2 - 300, self->window_size.y / 2 - 50, 50.0, GOLD);
    return;

    line_x = self->window_size.x / 2.0f;
    score_y = self->window_size.y / 2.0f - 25.0f;

    BeginDrawing();
    ClearBackground(BLACK);

    DrawLine(line_x, self->window_size.y, line_x, 0.0f, DARKGRAY);
    
    snprintf(buffer, sizeof buffer, "%u", self->score.left);
    DrawTextD(
        buffer, line_x - 75.0,
        score_y,
        50.0, GRAY
    );
    snprintf(buffer, sizeof buffer, "%u", self->score.right);
    DrawTextD(
        buffer, line_x + 50.0,
        score_y,
        50.0, GRAY
    );

    DrawCircleV(self->ball.pos, self->ball.radius, WHITE);

    DrawRectangleRec(
        self->paddle_left,
        RAYWHITE
    );
    DrawRectangleRec(
        self->paddle_right,
        RAYWHITE
    );
}

SelectedGame pong_update(Pong* self) {
    static float dt, fixed_paddle_speed;

    self->window_size = vec2(GetScreenWidth(), GetScreenHeight());
    self->paddle_right.x = self->window_size.x - PADDLE_WIDTH - PADDLE_PADDING;

    pong_draw(self);

    dt = GetFrameTime();

    if (IsKeyPressed(KEY_ESCAPE)) {
        return Selected_None;
    }
    return Selected_PONG; // NOTE remove later :p

    if (IsKeyPressed(KEY_H)) {
        self->state = (self->state == GameState_Help)? GameState_Running : GameState_Help;
        pong_draw(self);
        return Selected_PONG;
    }

    if (self->state == GameState_Help) {
        pong_draw_help(self);
        return Selected_PONG;
    }

    if (IsKeyPressed(KEY_R)) {
        self->score.left = 0;
        self->score.right = 0;
        self->state = GameState_Running;

        pong_reset(self, GetRandomValue(0, 1));
    }
    if (IsKeyPressed(KEY_SPACE)) {
        self->state = (self->state == GameState_Running) ? GameState_Paused : GameState_Running;
        pong_draw(self);
        return Selected_PONG;
    }

    if (self->state == GameState_Paused) {
        pong_draw(self);
        DrawTextD(
            "PAUSED",
            3, self->window_size.y - 25,
            25.0, GREEN
        );
        return Selected_PONG;
    }

    if (self->countdown_passed >= 0.0f) {
        self->countdown_passed =
            (self->countdown_passed >= 3.0f)?
            -1.0f : self->countdown_passed + dt;
    } else {
        fixed_paddle_speed = PADDLE_SPEED * dt;
        if (IsKeyDown(KEY_W)) {
            self->paddle_left.y = Clamp(
                self->paddle_left.y - fixed_paddle_speed,
                0.0f,
                self->window_size.y - PADDLE_HEIGHT
            );
        }
        if (IsKeyDown(KEY_S)) {
            self->paddle_left.y = Clamp(
                self->paddle_left.y + fixed_paddle_speed,
                0.0f,
                self->window_size.y - PADDLE_HEIGHT
            );
        }
        if (IsKeyDown(KEY_UP)) {
            self->paddle_right.y = Clamp(
                self->paddle_right.y - fixed_paddle_speed,
                0.0f,
                self->window_size.y - PADDLE_HEIGHT
            );
        }
        if (IsKeyDown(KEY_DOWN)) {
            self->paddle_right.y = Clamp(
                self->paddle_right.y + fixed_paddle_speed,
                0.0f,
                self->window_size.y - PADDLE_HEIGHT
            );
        }

    }

    if (self->countdown_passed < 0.0f) {
        self->ball.pos.x += self->ball.velocity.x * dt;
        self->ball.pos.y += self->ball.velocity.y * dt;

        pong_handle_collision(self);

        pong_draw(self);
    } else {
        pong_draw(self);

        static float x, y, size = 125.0;
        x = self->window_size.x / 2.0f - 30.0f;
        y = self->window_size.y / 2.0f- 70.0f;

        if (self->countdown_passed <= 1.0f)
            DrawTextD(
                "3", x, y, size, GREEN
            );
        else if (self->countdown_passed <= 2.0f)
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

void pong_deinit(Pong* self) {
    UnloadSound(self->hit_sound);
    UnloadSound(self->score_sound);
}


#endif