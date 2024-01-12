#ifndef BALL_H
#define BALL_H

#include "raylib.h"

#define BALL_RADIUS 5.0f
static const Vector2 BALL_DEFAULT_VELOCITY = (Vector2){250.0f, 250.0f};
static const Vector2 BALL_DEFAULT_NEG_VELOCITY = (Vector2){-250.0f, -250.0f};

typedef struct Ball {
    Vector2 pos;
    Vector2 velocity;
    float radius;
} Ball;

#endif