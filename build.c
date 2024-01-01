#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// utility
#define INFO(FMT, ...) printf("[INFO] " FMT "\n", ##__VA_ARGS__)
#define ERROR(MSG) perror("###########################\n[ERROR] " MSG)

int run_cmd(const char* command) {
    printf("[CMD] %s\n", command);
    return system(command);
}

// mkdir
#if defined(_WIN32)
#include <direct.h>
#define MKDIR(PATH) _mkdir(PATH)
#else
#include <sys/stat.h>
#define MKDIR(PATH) mkdir(PATH, 0755)
#endif

/************* Flags & Constants *************/
// the C compiler to use
#ifndef CC
#ifdef __APPLE__
#define CC "clang"
#else
#define CC "gcc"
#endif
#endif

// generic C compiler flags
#ifndef CFLAGS
#define CFLAGS " -lm -Wall -I include/ "
#endif

// for release mode
#ifndef CFLAGS_RELEASE
#define CFLAGS_RELEASE " -Ofast -s "
#endif

// windows-specific compiler flags
#ifndef CFLAGS_WIN
#define CFLAGS_WIN " ./winresource/resource.o lib/WIN32/libraylib.a -lwinmm -lgdi32 -lopengl32 -I include/external/deps/mingw -L lib/WIN32/ "
#endif

#ifndef RESOURCE_COMMAND
#define RESOURCE_COMMAND ".\\winresource\\windres.exe .\\winresource\\resource.rc -o .\\winresource\\resource.o"
#endif

// macos specific compiler flags (TODO)
#ifndef CFLAGS_APPLE
#define CFLAGS_APPLE " -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -L lib/macos/ lib/macos/libraylib.a "
#endif

// raylib linking flags
#ifndef RAYFLAGS
#define RAYFLAGS \
    " -D_GNU_SOURCE -std=gnu99 "\
    "-DGL_SILENCE_DEPRECATION=199309L "\
    "-I include/external/glfw/include "
#endif

#ifndef IN
#define IN " ./src/main.c "
#endif

#ifndef OUT
#define OUT " -o ./bin/MultiSim "
#endif

#if defined(_WIN32)
#  define COMPILE_CMD CC CFLAGS IN OUT RAYFLAGS CFLAGS_WIN
#elif defined(__APPLE__)
#  define COMPILE_CMD CC CFLAGS IN OUT RAYFLAGS CFLAGS_APPLE
#else
#  define COMPILE_CMD COMPILE_CMD " -static "
#endif

void print_usage(const char* program) {
    printf("USAGE: %s [release/debug]\n", program);
}

int main(int argc, char** argv) {
    const char* program = argv[0];
    
    bool release_mode = false;

    if (argc == 1) {
        fprintf(stderr, "ERROR: no argument provided.\n");
        print_usage(program);
        return 1;
    } else if (argc > 2) {
        fprintf(stderr, "ERROR: too many argument provided.\n");
        print_usage(program);
        return 1;
    }

    if (strcmp(argv[1], "release") == 0)
        release_mode = true;
    else if (strcmp(argv[1], "debug") == 0)
        release_mode = false;
    else {
        fprintf(stderr, "ERROR: unkown argument: `%s`.\n", argv[1]);
        print_usage(program);
        return 1;
    }

    // release_mode = true | false

#if defined(_WIN32)
    INFO("target OS = Windows");
#elif defined(__LINUX__)
    INFO("target os = Linux");
#elif defined(__APPLE__)
    INFO("target os = UNKOWN");
#endif

    INFO("release mode = %s", release_mode? "TRUE" : "FALSE");
    puts("-----------------------------");

    if (MKDIR("bin") == 0) {
        INFO("created ./bin/ folder.");
    } else INFO("./bin/ folder already exists.");

#ifdef _WIN32
    INFO("running resource command");
    if (run_cmd(RESOURCE_COMMAND) != 0) {
        ERROR("failed to run the resource command: ");
        return 1;
    }
#endif

    const char* command = COMPILE_CMD;
    if (release_mode) {
    #ifdef _WIN32
        command = COMPILE_CMD " -mwindows " CFLAGS_RELEASE;
    #else
        command = COMPILE_CMD CFLAGS_RELEASE;
    #endif
    } else {
        command = COMPILE_CMD " -D DEBUG ";
    }
    INFO("Compiling...");
    if (run_cmd(command) != 0) {
        ERROR("failed to compile: ");
        return 1;
    }
    puts("-----------------------------");
    INFO("Build successfull!");
    INFO("The executable is located in the ./bin/ folder.");
    return 0;
}