#ifndef CONST_H_
#define CONST_H_

// Incbin / binary files
#define INCBIN_PREFIX
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include "incbin.h"
#include "font.c"

INCBIN(bolus, "src/gol/bolus.png");
INCBIN(dvd_logo, "src/dvd/DVD_logo.png");

// expand X and turn it into a string literal
#define STRINGIFY(X) _STRINGIFY(X)
#define _STRINGIFY(X) #X

/* GOL */
#define GOL_SCALE 16
#define GOL_WINDOW_W 800
#define GOL_WINDOW_H 640

#define GOL_GRID_W (GOL_WINDOW_W / GOL_SCALE)
#define GOL_GRID_H (GOL_WINDOW_H / GOL_SCALE)

#define GOL_DEFAULT_TIME_STEP 0.05
#define GOL_DEFAULT_UPDATE_CAP 0.50
#define GOL_RESIZE_TIME_LIMIT 0.2

#endif