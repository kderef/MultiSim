#ifndef BALL_H
#define BALL_H

#include "raymath.h"

#define BALL_DEFAULT_V 150.0f
#define BALL_VX_INCREASE 1.1f
#define BALL_VY_INCREASE 1.2f

#define BALL_RADIUS 5.0f
static const Vector2 BALL_DEFAULT_VELOCITY = (Vector2){BALL_DEFAULT_V, BALL_DEFAULT_V};
static const Vector2 BALL_DEFAULT_NEG_VELOCITY = (Vector2){-BALL_DEFAULT_V, -BALL_DEFAULT_V};

typedef struct Ball {
    Vector2 pos;
    Vector2 velocity;
    float radius;
} Ball;

#endif