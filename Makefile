# Compiler settings
CC = gcc
CFLAGS = -lm -Wall -Wextra -I include/
CFLAGS_RELEASE = -Ofast -s
CFLAGS_DEBUG = -w -DDEBUG -Og
CFLAGS_WIN = ./winresource/resource.o lib/WIN32/libraylib.a -lwinmm -lgdi32 -lopengl32 -I include/external/deps/mingw -L lib/WIN32/ --static
CFLAGS_APPLE = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -L lib/macos/ lib/macos/libraylib.a
CFLAGS_LINUX = lib/linux/libraylib.a
RAYFLAGS = -D_GNU_SOURCE -DGL_SILENCE_DEPRECATION=199309L -I include/external/glfw/include
MINGWCC = x86_64-w64-mingw32-gcc
MINGW_RESOURCE_COMMAND = x86_64-w64-mingw32-windres winresource/resource.rc -o winresource/resource.o
RESOURCE_COMMAND = .\\winresource\\windres.exe .\\winresource\\resource.rc -o .\\winresource\\resource.o
MKDIR_P = mkdir -p "
RM_RF = rm -rf "

# OS detection
ifeq ($(OS),Windows_NT)
	OS_FLAGS = $(CFLAGS_WIN)
	CFLAGS_RELEASE += -mwindows -Wl,--strip-debug
	MKDIR_P = cmd.exe /c "md
	RM_RF = cmd.exe "/c DEL /f /s /q
else
	RESOURCE_COMMAND = 
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OS_FLAGS = $(CFLAGS_LINUX)
		CFLAGS_RELEASE += -Wl,--strip-debug
	endif
	ifeq ($(UNAME_S),Darwin)
		OS_FLAGS = $(CFLAGS_APPLE)
	endif
endif

# Targets
BIN = ./bin/MultiSim
MAIN = ./src/main.c

# Default target
all: release

# Build modes
release: CFLAGS += $(CFLAGS_RELEASE) $(OS_FLAGS)
release: build_resource build

debug: CFLAGS += $(CFLAGS_DEBUG) $(OS_FLAGS)
debug: build

mingw: CC = $(MINGWCC)
mingw: RESOURCE_COMMAND = $(MINGW_RESOURCE_COMMAND)
mingw: CFLAGS += $(CFLAGS_WIN)
mingw: build_resource build

# Compile and link
build:
	-$(MKDIR_P) bin"
	$(CC) $(MAIN) -o $(BIN) $(CFLAGS) $(RAYFLAGS)

build_resource:
	$(RESOURCE_COMMAND)

# Run target
run:
	./bin/MultiSim

# Clean target
clean:
	$(RM_RF) bin/MultiSim winresource/resource.o"
	$(RM_RF) bin"

# Usage
.PHONY: all release debug mingw build build_resource run clean
