#ifndef PONG_C_
#define PONG_C_

#include "raylib.h"
#include "raymath.h"
#include "../const.h"
#include "../gamestate.c"

INCBIN(pong_hit_sound, "assets/beep.wav");

#define BALL_RADIUS 5.0f
#define PADDLE_WIDTH 10.0f
#define PADDLE_HEIGHT 100.0f
#define PADDLE_PADDING 5.0f
#define PADDLE_SPEED 150.0f
#define BALL_DEFAULT_VELOCITY (Vector2){250.0f, 250.0f}

typedef struct {
    unsigned int left;
    unsigned int right;
} Score;

typedef enum {
    PONG_Paused,
    PONG_Running,
    PONG_Helpmode,
} PongGameState;

typedef struct {
    Rectangle paddle_left;
    Rectangle paddle_right;
    Sound hit_sound;
    Sound score_sound;
    Score score;
    PongGameState state;
    float countdown_passed;
    Vector2 ball_pos;
    Vector2 ball_velocity;
    Vector2 window_size;
    bool paused;
} Pong;

Pong pong_new() {
    Pong p;
    p.window_size = vec2(GetScreenWidth(), GetScreenHeight());
    Vector2 center;
    center.x = p.window_size.x / 2;
    center.y = p.window_size.y / 2;

    float paddle_y = center.y - PADDLE_HEIGHT / 2.0f;

    p.ball_pos = center;
    p.ball_velocity = BALL_DEFAULT_VELOCITY;
    p.score = (Score) {0, 0};
    p.paused = false;

    p.paddle_left = rect(
        PADDLE_PADDING, paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT
    );
    p.paddle_right = rect(
        p.window_size.x - PADDLE_PADDING - PADDLE_WIDTH, paddle_y,
        PADDLE_WIDTH,
        PADDLE_HEIGHT
    );
    p.state = PONG_Helpmode;
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

void pong_reset(Pong* self, bool left_scored) {
    static float paddle_y, abs_vx;
    paddle_y = self->window_size.y*0.5 - PADDLE_HEIGHT*0.5;

    self->paddle_right.y = paddle_y;
    self->paddle_left.y = paddle_y;
    self->ball_pos = vec2(
        left_scored?
            PADDLE_PADDING*2 + PADDLE_WIDTH :
            self->window_size.x - PADDLE_PADDING*2 - PADDLE_WIDTH,
        self->window_size.y / 2.0f
    );

    abs_vx = fabsf(self->ball_velocity.x);

    self->ball_velocity.x = left_scored? abs_vx : -abs_vx;
    self->ball_velocity.y *= GetRandomValue(0, 1)? 1.0f : -1.0f;

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

    DrawCircleV(self->ball_pos, BALL_RADIUS, WHITE);

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

    dt = GetFrameTime();

    if (IsKeyPressed(KEY_ESCAPE)) {
        return Selected_None;
    }
    if (IsKeyPressed(KEY_H)) {
        self->state = (self->state == PONG_Helpmode)? PONG_Paused : PONG_Helpmode;
        pong_draw(self);
        EndDrawing();
        return Selected_PONG;
    }

    if (self->state == PONG_Helpmode) {
        pong_draw_help(self);
        EndDrawing();
        return Selected_PONG;
    }

    if (IsKeyPressed(KEY_R)) {
        self->score.left = 0;
        self->score.right = 0;
        self->paused = false;

        pong_reset(self, GetRandomValue(0, 1));
    }
    if (IsKeyPressed(KEY_SPACE)) {
        self->paused = !(self->paused);
        pong_draw(self);
        EndDrawing();
        return Selected_PONG;
    }

    if (self->paused) {
        pong_draw(self);
        DrawTextD(
            "PAUSED",
            3, self->window_size.y - 25,
            25.0, GREEN
        );
        EndDrawing();
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
        self->ball_pos.x += self->ball_velocity.x * dt;
        self->ball_pos.y += self->ball_velocity.y * dt;

        if (CheckCollisionCircleRec(self->ball_pos, BALL_RADIUS, self->paddle_left) ||
            CheckCollisionCircleRec(self->ball_pos, BALL_RADIUS, self->paddle_right)) {

            // Horizontal collision with left paddle
            if (self->ball_velocity.x < 0.0f &&
                self->ball_pos.x - BALL_RADIUS <= self->paddle_left.x + PADDLE_WIDTH &&
                self->ball_pos.y >= self->paddle_left.y - BALL_RADIUS &&
                self->ball_pos.y <= self->paddle_left.y + PADDLE_HEIGHT + BALL_RADIUS) {
                PlaySound(self->hit_sound);
                self->ball_velocity.x = -(self->ball_velocity.x); // Reverse x velocity
            }

            // Horizontal collision with right paddle
            else if (self->ball_velocity.x > 0.0f &&
                self->ball_pos.x + BALL_RADIUS >= self->paddle_right.x &&
                self->ball_pos.y >= self->paddle_right.y - BALL_RADIUS &&
                self->ball_pos.y <= self->paddle_right.y + PADDLE_HEIGHT + BALL_RADIUS) {
                PlaySound(self->hit_sound);
                self->ball_velocity.x = -fabsf(self->ball_velocity.x); // Reverse x velocity
            }

            // Vertical collision with paddles
            if (self->ball_velocity.y < 0.0f &&
                self->ball_pos.y - BALL_RADIUS <= self->paddle_left.y + PADDLE_HEIGHT &&
                self->ball_pos.x >= self->paddle_left.x - BALL_RADIUS &&
                self->ball_pos.x <= self->paddle_left.x + PADDLE_WIDTH + BALL_RADIUS) {
                PlaySound(self->hit_sound);
                self->ball_velocity.y = fabsf(self->ball_velocity.y); // Reverse y velocity
            }

            if (self->ball_velocity.y > 0.0f &&
                self->ball_pos.y + BALL_RADIUS >= self->paddle_right.y &&
                self->ball_pos.x >= self->paddle_right.x - BALL_RADIUS &&
                self->ball_pos.x <= self->paddle_right.x + PADDLE_WIDTH + BALL_RADIUS) {
                PlaySound(self->hit_sound);
                self->ball_velocity.y = -fabsf(self->ball_velocity.y); // Reverse y velocity
            }
        }
        else {
            if (self->ball_pos.y + BALL_RADIUS >= self->window_size.y || self->ball_pos.y - BALL_RADIUS <= 0.0f)
            {
                PlaySound(self->hit_sound);
                self->ball_velocity.y = -(self->ball_velocity.y);
            }
            else
                if (self->ball_pos.x - BALL_RADIUS <= 0.0f) {
                    PlaySound(self->score_sound);
                    self->score.right++;
                    pong_reset(self, false);
                }
                else if (self->ball_pos.x + BALL_RADIUS >= self->window_size.x) {
                    PlaySound(self->score_sound);
                    self->score.left++;
                    pong_reset(self, true);
                }
        }

        pong_draw(self);
        EndDrawing();
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
        EndDrawing();
    }
    return Selected_PONG;
}

void pong_deinit(Pong* self) {
    UnloadSound(self->hit_sound);
}


#endif