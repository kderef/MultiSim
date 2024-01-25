#ifndef PANIC_H
#define PANIC_H

#include <stdbool.h>
#include <signal.h>
#include "const.h"

static bool paniced = false;
static const char* panic_loc = "Unkown panic location.";
static const char* panic_msg = "No panic message provided.";

void panic_with_loc(const char*, const char*);

void panic_with_loc(const char* loc, const char* msg) {
    panic_msg = msg;
    panic_loc = loc;
    paniced = true;
}

/// include location in panic
#if defined(__GNUC__)
#  define PANIC_FILE_MACRO __BASE_FILE__
#elif defined(__clang__)
#  define PANIC_FILE_MACRO __FILE_NAME__
#else
#  define PANIC_FILE_MACRO __FILE__
#endif

#define panic(MSG) panic_with_loc(PANIC_FILE_MACRO":"STRINGIFY(__LINE__), MSG)

#endif