#define VERSION "2.0.6-dev"

// reduce file size on windows
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#endif

#include "raylib.h"
#include "const.h"
#include "panic.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#include "ui/selector.c"
#include "ui/windowicon.c"
#include "ui/splashtext.c"

static Selector* selector_cleanup = NULL;

void strupper(char* s) {
    const char OFFSET = 'a' - 'A';
    while (*s) {
        *s = (*s >= 'a' && *s <= 'z') ? *s -= OFFSET : *s;
        s++;
    }
}

/// Handle 'panic' (global variable paniced = true)
void panic_handler(Selector* s) {
    char dump_buf[4112] = "";
    char dump_buf_c[2056] = "";

    SetTraceLogLevel(LOG_ALL);

    fprintf(stderr, "[PANIC] at location %s, message: %s\n", panic_loc, panic_msg);
    fprintf(stderr, "--> Cleaning up...\n");

    // cleanup
    CloseWindow();
    selector_deinit(s);
    unload_default_font();

    char log_file[FILENAME_MAX];
    snprintf(log_file, sizeof log_file, "multisim-%lld.log", time(NULL));

    fprintf(stderr, "\nWriting to log file \"%s\"...\n", log_file);

    #define DUMP_SPRINTF(...) snprintf(dump_buf_c, sizeof dump_buf_c, __VA_ARGS__); strcat(dump_buf, dump_buf_c)

    DUMP_SPRINTF("*** PANIC REPORT ***\n");
    DUMP_SPRINTF("* Location: %s\n", panic_loc);
    DUMP_SPRINTF("* Message:  %s\n", panic_msg);
    DUMP_SPRINTF("* File:     %s\n", log_file);

    SaveFileText(log_file, dump_buf);

    exit(0);
}

/// Handle signal `sig` and clean resources up.
void signal_handler(int sig) {
    char signal_str[64] = "SIG";
#ifdef _WIN32
    char* _siglist[] = {
        [SIGABRT] = "SIGABRT",
        [SIGFPE] = "SIGFPE",
        [SIGILL] = "SIGILL",
        [SIGINT] = "SIGINT",
        [SIGSEGV] = "SIGSEGV",
        [SIGTERM] = "SIGTERM"
    };
    strcpy(signal_str, _siglist[sig]);
#else
    strcat(signal_str, strsignal(sig));
    strupper(signal_str + 3);
    
#endif
    fprintf(stderr, "\nWARNING: Signal %s caught, cleaning up...\n======================================================\n", signal_str);

    SetTraceLogLevel(LOG_ALL);

    // cleanup
    CloseWindow();
    selector_deinit(selector_cleanup);
    unload_default_font();
        
    exit(0);
}

int main(void) {
    // seed random
    SetRandomSeed(time(NULL));

    // initialize audio backend
    InitAudioDevice();

    // raylib initialization
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_W, WINDOW_H, selected_get_window_title(Selected_None));
    SetExitKey(KEY_NULL);
    SetWindowMinSize(WINDOW_W, WINDOW_H);
    SetTargetFPS(
        GetMonitorRefreshRate(
            GetCurrentMonitor()
        ) * 2
    );

    // load the default font (bundled IN exe)
    load_default_font();

    // set the window icon
    load_window_icon();

    // load the random splash text
    load_random_splash_text();

    // the selector manages all the games and renders the title screen
    Selector* selector = selector_init();
    selector_cleanup = selector;

    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);

    // if release mode, disable all logging messages except for LOG_ERROR
#ifndef DEBUG
    SetTraceLogLevel(LOG_ERROR);
#endif

    while (!WindowShouldClose()) {
        if (paniced) panic_handler(selector);
        selector_update(selector);
    }

    // close the window and unload all assets (textures, images, etc.)
    CloseWindow();
    selector_deinit(selector);
    unload_default_font();

    return 0;
}