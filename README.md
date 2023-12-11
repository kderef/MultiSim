# Game-Of-Life
**A rust implementation of Conways Game of Life**.


## What is Game of Life?
Conways Game of Life is a simple [cellular automaton](https://en.wikipedia.org/wiki/Cellular_automaton) that consists of just **4 simple rules:**
```
1: Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2: Any live cell with two or three live neighbours lives on to the next generation.
3: Any live cell with more than three live neighbours dies, as if by overpopulation.
4: Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
```
This implementation is written in Rust using the [raylib-rs](https://github.com/deltaphc/raylib-rs) package, a port of the [Raylib Library for C](https://www.raylib.com/)

---
## Installation instructions
You can either download the compiled binaries from the [releases tab](releases)
or **build from source**

### Building from source
Before installing you need to have [Cargo](https://doc.rust-lang.org/book/ch01-03-hello-cargo.html) installed on your system.
```
git clone https://github.com/kn-ht/game-of-life
cd game-of-life
cargo build --release
```
The compiled binary will be in `./target/release`
