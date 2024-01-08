#ifndef CONST_H_
#define CONST_H_

#include <stdint.h>

// Incbin / binary files
#define INCBIN_SILENCE_BITCODE_WARNING
#define INCBIN_PREFIX
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include "external/incbin.h"

// expand X and turn it into a string literal
#define STRINGIFY(X) _STRINGIFY(X)
#define _STRINGIFY(X) #X

// easier initialization
#define rect(X, Y, W, H) (Rectangle){(float)(X), (float)(Y), (float)(W), (float)(H)}
#define vec2(X, Y) (Vector2){(float)(X), (float)(Y)}

/* GOL */
constexpr int GOL_SCALE = 16;
constexpr int WINDOW_W = 800;
constexpr int WINDOW_H = 648;

constexpr int GOL_STATUS_BAR_HEIGHT = 40;

constexpr int GOL_GRID_W  = (WINDOW_W / GOL_SCALE);
constexpr int GOL_GRID_H  = (WINDOW_H - GOL_STATUS_BAR_HEIGHT) / GOL_SCALE;

constexpr float GOL_DEFAULT_TIME_STEP = 0.01f;
constexpr float GOL_DEFAULT_UPDATE_CAP = 0.2f;
constexpr float GOL_RESIZE_TIME_LIMIT = 0.2f;

typedef uint_fast8_t u8;
typedef int_fast8_t i8;

#endif