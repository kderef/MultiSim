mod rl_port;
mod gol;

use raylib::prelude::*;
use rl_port::*;

use gol::{
    WINDOW_W, WINDOW_H,
    GameOfLife,
};

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(WINDOW_W as i32, WINDOW_H as i32)
        .title("Game of Life")
        .build();

    let mut gol = GameOfLife::new(rl, thread);

    gol.run();
}
