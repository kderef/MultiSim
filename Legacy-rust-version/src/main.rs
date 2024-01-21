#![allow(non_snake_case)]
#![cfg_attr(all(target_os = "windows", not(debug_assertions)), windows_subsystem = "windows")]

/*
use dvd::Dvd;
use raylib_ffi::colors;
use raylib_port::{init_audio_device, Window, begin_drawing, clear_background, end_drawing, Image, Rectangle};

mod game;
mod gamestate;

// ports of unsafe code
mod  raylib_port;

mod gol_consts;
mod dvd;


// game trait
mod game;

// Selector for the games
mod selector;

// Pong
mod pong;

// Game of Life
mod gol_cell;
mod gol_consts;
mod gol_game;
mod gol_theme;
mod gol_universe;

// DVD bouncing
mod dvd;

use gol_consts::{WINDOW_H, WINDOW_W};
use selector::Selector;

use std::time::{UNIX_EPOCH, SystemTime};
*/

use raylib::{drawing::RaylibDraw, color::Color};

fn main() {
    let (mut rl, th) = raylib::init()
        .width(600).height(500).build();

    while !rl.window_should_close() {
        let mut dr = rl.begin_drawing(&th);
        dr.clear_background(Color::BLACK);

        for y in 0..50 {
            for x in 0..60 {
                dr.draw_text("#", x, y, 100, Color::GRAY);
            }
        }
        dr.draw_fps(0, 0);
    }
}
