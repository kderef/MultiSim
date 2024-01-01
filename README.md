# MultiSim
*A game collection written in C using [Raylib](https://raylib.com)*.
---

## CONTROLS
- **Space** to pause/unpause the current game.
- **Escape** to return to the main menu.
- **H** to toggle help mode.
---
## INSTALLING
You can download the exe from the releases tab on the right of the page,
or you can **build it from source**.
---
#### Building from source
Before building you need to have installed on your system:
- [git](https://git-scm.com/book/en/v2/Getting-Started-The-Command-Line)
- A C compiler. It works out of the box with [Mingw](https://www.mingw-w64.org/), [GCC](https://gcc.gnu.org/) and [Clang](https://clang.llvm.org/).

Then run the following command in your terminal.
**NOTE**: replace *{CC}* with your C compiler of choice (*gcc, clang, etc.*).
**NOTE**: When cross compiling with mingw, replace _build.c_ with _build-mingw.c_.
```
git clone https://github.com/kn-ht/multisim
cd multisim
{CC} build.c -o build
./build release
```

The compiled executable will be in the ./bin/ folder.
