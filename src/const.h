#ifndef CONST_H_
#define CONST_H_

#include <stdint.h>

// URL to the repository
#define GIT_URL "https://github.com/kn-ht/multisim"

// raylib defines
#define vec2(X, Y) (Vector2){(float)(X), (float)(Y)}
#define vec3(X, Y, Z) (Vector3){(float)(X), (float)(Y), (float)(Z)}
#define rect(X, Y, W, H) (Rectangle){(float)(X), (float)(Y), (float)(W), (float)(H)}

#define VEC2_ZERO (Vector2){0.0f, 0.0f}
#define VEC3_ZERO (Vector3){0.0f, 0.0f, 0.0f}
#define RECT_ZERO (Rectangle){0.0f, 0.0f, 0.0f, 0.0f}

// Incbin / binary files
#define INCBIN_SILENCE_BITCODE_WARNING
#define INCBIN_PREFIX
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include "external/incbin.h"

// expand X and turn it into a string literal
#define STRINGIFY(X) _STRINGIFY(X)
#define _STRINGIFY(X) #X

// easier initialization
#define min(a, b) (((a) < (b))? (a) : (b))
#define max(a, b) (((a) > (b))? (a) : (b))

// construct Color
#define color(R, G, B) \
    (Color){(uint8_t)(R), (uint8_t)(G), (uint8_t)(B), (uint8_t)255}

// construct rgba from hex
// example: color_hex(0xffffff) -> Color{r = 255, g = 255, ...}
#define color_hex(H) \
    (Color){((H) >> 24) & 0xFF, ((H) >> 16) & 0xFF, ((H) >> 8) & 0xFF, (uint8_t)255}

/* GOL */
#define GOL_SCALE  14
#define WINDOW_W   (GOL_SCALE * 57)
#define WINDOW_H   (GOL_SCALE * 46) 

#define GOL_STATUS_BAR_HEIGHT 40

#define GOL_GRID_W  (WINDOW_W / GOL_SCALE)
#define GOL_GRID_H  (WINDOW_H - GOL_STATUS_BAR_HEIGHT) / GOL_SCALE

#define GOL_DEFAULT_TIME_STEP   0.001f
#define GOL_DEFAULT_UPDATE_CAP  0.2f
#define GOL_RESIZE_TIME_LIMIT   0.2f

typedef uint_fast8_t u8;
typedef int_fast8_t i8;

#endif
