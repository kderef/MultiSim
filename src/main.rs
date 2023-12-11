#![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

mod gol;
use gol::{
    WINDOW_W, WINDOW_H,
    GameOfLife, TITLE_DESIGN_MODE,
};

/* ### Conways Game of Life ###
    1: Any live cell with fewer than two live neighbours dies, as if by underpopulation.
    2: Any live cell with two or three live neighbours lives on to the next generation.
    3: Any live cell with more than three live neighbours dies, as if by overpopulation.
    4: Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
*/

fn main() {
    let (rl, thread) = raylib::init()
        .size(WINDOW_W as i32, WINDOW_H as i32)
        .title(TITLE_DESIGN_MODE)
        .build();

    let mut gol = GameOfLife::new(rl, thread);

    gol.prepare();
    gol.run();
}
