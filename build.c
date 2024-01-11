// your configuration goes here.
// example:
//#define CC "x86_64-w64-mingw32-gcc"
//#define OUT " -o ./bin/HelloWorld "
// For default values see build.h


// for easily switching MingW compilation on/off.
// set to 1 for cross compilation with mingw (replace CC as needed)
#if 0
    #define CC "x86_64-w64-mingw32-gcc"
    #define RESOURCE_COMMAND "x86_64-w64-mingw32-windres winresource/resource.rc -o winresource/resource.o"
    #define _WIN32 1
    #define __APPLE__ 0
    #define __LINUX__ 0
#endif

#include "build.h"

void print_usage(const char* program) {
    printf("USAGE: %s [release/debug]\n", program);
}

int main(int argc, char** argv) {
    bool release_mode = false;
    const char* program = argv[0];

    if (argc == 1) {
        ERROR("no argument provided.");
        print_usage(program);
        return 1;
    } else if (argc > 2) {
        ERROR("ERROR: too many argument provided.");
        print_usage(program);
        return 1;
    }

    if (strcmp(argv[1], "release") == 0) release_mode = true;
    else if (strcmp(argv[1], "debug") == 0) release_mode = false;
    else {
        ERROR("unkown argument: `%s`.", argv[1]);
        print_usage(program);
        return 1;
    }

    return build(release_mode, argv[0]);
}