#ifndef BALL_H
#define BALL_H

#define BALL_DEFAULT_VELOCITY (Vector2){250.0f, 250.0f}
#define BALL_DEFAULT_NEG_VELOCITY (Vector2){-250.0f, -250.0f}
#define BALL_RADIUS 5.0f

typedef struct {
    Vector2 pos;
    Vector2 velocity;
    float radius;
} Ball;

#endif