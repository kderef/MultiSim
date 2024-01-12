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
template<typename T>
constexpr Vector2 vec2(T x, T y) {
    return (Vector2){static_cast<float>(x), static_cast<float>(y)};
}

template<typename T>
constexpr Rectangle rect(T x, T y, T w, T h) {
    return (Rectangle) {
        static_cast<float>(x), static_cast<float>(y),
        static_cast<float>(w), static_cast<float>(h)
    };
}

template<typename T>
constexpr T min(T a, T b) {
    return (a < b)? a : b;
}

template<typename T>
constexpr T max(T a, T b) {
    return (a > b)? a : b;
}

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