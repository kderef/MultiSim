#ifndef BALL_H
#define BALL_H

#include "raylib.h"

constexpr float BALL_RADIUS = 5.0f;
constexpr Vector2 BALL_DEFAULT_VELOCITY = (Vector2){250.0f, 250.0f};
constexpr Vector2 BALL_DEFAULT_NEG_VELOCITY = (Vector2){-250.0f, -250.0f};

struct Ball {
    Vector2 pos;
    Vector2 velocity;
    float radius;
};

#endif