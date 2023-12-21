#![allow(non_snake_case)]
#![cfg_attr(all(target_os = "windows", not(debug_assertions)), windows_subsystem = "windows")]

// game trait
mod game;
use game::Game;

// Selector for the games
mod selector;

// Game of Life
mod gol_cell;
mod gol_consts;
mod gol_game;
mod gol_theme;
mod gol_universe;

// DVD bouncing
mod dvd;

use gol_consts::{WINDOW_H, WINDOW_W};
use gol_game::GameOfLife;
use selector::{Selector, SelectedGame};
use dvd::Dvd;

use std::time::{UNIX_EPOCH, SystemTime};
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

    // instantiate games
    let mut selector = Selector::new();
    let mut current_game = selector::SelectedGame::None;

    let mut gol = GameOfLife::new();
    let mut dvd = Dvd::new();

    loop {
        current_game = match current_game {
            SelectedGame::Dvd => dvd.update(),
            SelectedGame::GameOfLife => gol.update(),
            SelectedGame::None => selector.update()
        };

        next_frame().await;
    }
}
