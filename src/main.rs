#![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

mod gol_cell;
mod gol_consts;
mod gol_game;
mod gol_theme;
mod gol_universe;

use std::time::{UNIX_EPOCH, SystemTime};

use gol_consts::{WINDOW_H, WINDOW_W};
use gol_game::GameOfLife;
use macroquad::{miniquad::conf::Icon, prelude::*};

fn window_conf() -> Conf {
    Conf {
        window_title: "MultiSim-rs".to_owned(),
        window_width: WINDOW_W as i32,
        window_height: WINDOW_H as i32,
        high_dpi: true,
        fullscreen: false,
        window_resizable: true,
        sample_count: Default::default(),
        icon: Some(Icon::miniquad_logo()),
        platform: Default::default(),
    }
}

#[macroquad::main(window_conf)]
async fn main() {
    // seed rand
    macroquad::rand::srand(
        SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .and_then(|t| Ok(t.as_secs()))
            .unwrap_or(69),
    );

    // instantiate game
    let mut gol = GameOfLife::new();
    gol.update_screen_size();

    loop {
        gol.run();
        next_frame().await;
    }
}
