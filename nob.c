// build system
#define NOBUILD_IMPLEMENTATION
#include "nob.h"

// string comparison
#define streq(A, B) (strcmp((A), (B)) == 0)

#ifdef _WIN32
#  define IS_WINDOWS 1
#else
#  define IS_WINDOWS 0
#endif

#ifdef __APPLE__
#  define IS_APPLE 1
#else
#  define IS_APPLE 0
#endif

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

// for debug mode
#ifndef CFLAGS_DEBUG
#  define CFLAGS_DEBUG " -w -DDEBUG -Og "
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
#  if IS_WINDOWS
#    define BIN ".\\bin\\MultiSim.exe"
#  else
#    define BIN "./bin/MultiSim"
#  endif
#endif

// mkdir()
#if IS_WINDOWS
#  include <direct.h>
#  define MKDIR(_PATH) _mkdir(_PATH)
#else
#  define MKDIR(_PATH) mkdir(_PATH, 0755)
#endif

typedef enum {
    BUILD_NONE,
    BUILD_DEBUG,
    BUILD_RELEASE,
    BUILD_WEB
} BuildMode;

void print_usage(const char* program) {
    fprintf(stderr, "USAGE: %s [release/debug] [run]\n", program);
}

int main(int argc, char** argv) {
    GO_REBUILD_URSELF(argc, argv);

    const char* cc = CC;
    char cflags[1024] = CFLAGS;

    const char* resource_command = RESOURCE_COMMAND;

    BuildMode build_mode = BUILD_NONE;
    bool mingw = false, run = false;
    const char* program = argv[0];

    #define BIN_FOLDER "." PATH_SEP "bin"
    if (!path_is_dir(BIN_FOLDER)) {
        INFO("Creating dir: " BIN_FOLDER);
        MKDIR(BIN_FOLDER);
    }

    for (int i = 1; i < argc; i++) {
        if (streq(argv[i], "release")) {
            build_mode = BUILD_RELEASE;
        }
        else if (streq(argv[i], "debug")) {
            build_mode = BUILD_DEBUG;
        }
        else if (streq(argv[i], "mingw")) {
            mingw = true;
        }
        else if (streq(argv[i], "run")) {
            run = true;
        }
        else if (streq(argv[i], "web")) {
            build_mode = BUILD_WEB;
            cc = "emcc";
        }
        else {
            ERRO("Unkown argument: %s", argv[i]);
            return 1;
        }
    }

    if (mingw) {
        cc = MINGWCC;
        resource_command = MINGW_RESOURCE_COMMAND;
        strcat(cflags, CFLAGS_WIN);
    }
    switch (build_mode) {
        case BUILD_NONE: {
            ERRO("No build mode selected.");
            print_usage(program);
            return 1;
        } break;
        case BUILD_RELEASE: {
            strcat(cflags, CFLAGS_RELEASE);
            if (mingw || IS_WINDOWS) {
                strcat(cflags, " -mwindows "); // remove console window
            }
        } break;
        case BUILD_DEBUG: {
            strcat(cflags, CFLAGS_DEBUG);
        } break;
    }

    if (!mingw) {
        if (IS_WINDOWS) strcat(cflags, CFLAGS_WIN);
        else if (IS_APPLE) strcat(cflags, CFLAGS_APPLE);
    }

    strcat(cflags, RAYFLAGS);

    char command[2056];

    if (mingw || IS_WINDOWS) {
        INFO("CMD(winresource): %s", resource_command);
        int resource_success = system(resource_command);

        if (resource_success != 0) {
            ERRO("resource command `%s` exited with non-zero code %d.", resource_command, resource_success);
            return 1;
        }

        printf("\n=================================\n");
    }

    snprintf(
        command, sizeof command,
        "%s %s -o %s %s",
        cc, MAIN, BIN, cflags
    );

    INFO("CC     : %s", cc);
    INFO("CFLAGS : %s\n", cflags);

    INFO("CMD: %s", command);
    int comp_result = system(command);

    if (comp_result != 0) {
        ERRO("Compile command `%s` exited with non-zero code %d", command, comp_result);
        INFO("Aborting...");
        return 1;
    }

    if (run) {
        if (mingw) {
            ERRO("Can't combine 'run' with 'mingw'. You can run the executable with wine or some other emulation program, if desired.");
        }
        else {
            INFO("Running " BIN);
            system(BIN);
        }
    }

    return 0;
}