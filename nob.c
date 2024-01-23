#define NOBUILD_IMPLEMENTATION
#include "nob.h"

#include <stdbool.h>

/************* Flags & Constants *************/
// the C compiler to use

#ifndef CC
#  ifdef __APPLE__
#    define CC "clang"
#  else
#    define CC "gcc"
#  endif
#endif

// mingw cross-compile flasgs
#ifndef MINGWCC
#  define MINGWCC "x86_64-w64-mingw32-gcc"
#endif

#ifndef MINGW_RESOURCE_COMMAND
#  define MINGW_RESOURCE_COMMAND "x86_64-w64-mingw32-windres winresource/resource.rc -o winresource/resource.o"
#endif

// generic C compiler flags
#ifndef CFLAGS
#  define CFLAGS " -lm -Wall -Wextra -I include/ "
#endif

// for release mode
#ifndef CFLAGS_RELEASE
#  define CFLAGS_RELEASE " -Ofast -s "
#endif

// windows-specific compiler flags
#ifndef CFLAGS_WIN
#  define CFLAGS_WIN " ./winresource/resource.o lib/WIN32/libraylib.a -lwinmm -lgdi32 -lopengl32 -I include/external/deps/mingw -L lib/WIN32/ --static "
#endif

#ifndef RESOURCE_COMMAND
#  define RESOURCE_COMMAND ".\\winresource\\windres.exe .\\winresource\\resource.rc -o .\\winresource\\resource.o"
#endif

// macos specific compiler flags
#ifndef CFLAGS_APPLE
#  define CFLAGS_APPLE " -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -L lib/macos/ lib/macos/libraylib.a "
#endif

// raylib linking flags
#ifndef RAYFLAGS
#  define RAYFLAGS \
    " -D_GNU_SOURCE "\
    "-DGL_SILENCE_DEPRECATION=199309L "\
    "-I include/external/glfw/include "
#endif

#ifndef MAIN
#  define MAIN " ./src/main.c "
#endif

#ifndef BIN
#  ifdef _WIN32
#    define BIN ".\\bin\\MultiSim.exe"
#  else
#    define BIN "./bin/MultiSim"
#  endif
#endif

// mkdir()
#ifdef _WIN32
#  define MKDIR(_PATH) _mkdir(_PATH)
#else
#  define MKDIR(_PATH) mkdir(_PATH, 0755)
#endif

void print_usage(const char* program) {
    fprintf(stderr, "USAGE: %s [release/debug]\n", program);
}

int main(int argc, char** argv) {
    GO_REBUILD_URSELF(argc, argv);

    const char* cc = CC;
    char cflags[1024] = CFLAGS;
    char cflags[1024] = CFLAGS;
    const char* resource_command = RESOURCE_COMMAND;

    bool release_mode = false, mingw = false, run = false;
    const char* program = argv[0];

    #define BIN_FOLDER "." PATH_SEP "bin"
    if (!path_is_dir(BIN_FOLDER)) {
        INFO("Creating dir: " BIN_FOLDER);
        MKDIR(BIN_FOLDER);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "release") == 0) {
            release_mode = true;
        }
        else if (strcmp(argv[i], "debug") == 0) {
            release_mode = false;
        }
        else if (strcmp(argv[i], "mingw") == 0) {
            mingw = true;
        }
        else if (strcmp(argv[i], "run") == 0) {
            run = true;
        }
        else {
            WARN("Unkown argument: %s", argv[i]);
        }
    }

    if (mingw) {
        cc = MINGWCC;
        resource_command = MINGW_RESOURCE_COMMAND;
        strcat(cflags, CFLAGS_WIN);
    }
    if (release_mode) {
        strcat(cflags, CFLAGS_RELEASE);
        if (mingw) {
            strcat(cflags, " -mwindows ");
        }
    } else {
        strcat(cflags, " -DDEBUG ");
    }


    if (!mingw)
#if defined(_WIN32)
        strcat(cflags, CFLAGS_WIN);
#elif defined(__APPLE__)
        strcat(cflags, CFLAGS_APPLE);
#endif

    strcat(cflags, RAYFLAGS);

    char command[2056];

    INFO("CMD(winresource): %s", resource_command);
    int resource_success = system(resource_command);

    snprintf(
        command, sizeof command,
        "%s %s -o %s %s",
        cc, MAIN, BIN, cflags
    );

    INFO("CMD: %s", command);
    int comp_result = system(command);

    if (comp_result != 0) {
        ERRO("Compile command `%s` exited with non-zero code %d", command, comp_result);
        INFO("Aborting...");
        return 1;
    }
    int comp_result = system(command);

    if (comp_result != 0) {
        ERRO("Compile command `%s` exited with non-zero code %d", command, comp_result);
        INFO("Aborting...");
        return 1;
    }

    if (run) {
        INFO("Running " BIN);
        system(BIN);
    }

    return 0;
}