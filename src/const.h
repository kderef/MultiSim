#ifndef CONST_H_
#define CONST_H_

// Incbin / binary files
#define INCBIN_SILENCE_BITCODE_WARNING
#define INCBIN_PREFIX
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include "external/incbin.h"

INCBIN(bolus, "assets/bolus.png");

// expand X and turn it into a string literal
#define STRINGIFY(X) _STRINGIFY(X)
#define _STRINGIFY(X) #X

// easier initialization
#define rect(X, Y, W, H) (Rectangle){X, Y, W, H}
#define vec2(X, Y) (Vector2){X, Y}

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

/* GOL */
#define GOL_SCALE 16
#define WINDOW_W 800
#define WINDOW_H 640

#define GOL_GRID_W (WINDOW_W / GOL_SCALE)
#define GOL_GRID_H (WINDOW_H / GOL_SCALE)

#define GOL_DEFAULT_TIME_STEP 0.05
#define GOL_DEFAULT_UPDATE_CAP 0.50
#define GOL_RESIZE_TIME_LIMIT 0.2

#endif