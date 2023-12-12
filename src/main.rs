#![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

mod gol;
use gol::{GameOfLife, WINDOW_H, WINDOW_W};
use macroquad::prelude::*;

fn window_conf() -> Conf {
    Conf {
        window_title: "Game of Life | H - Help".to_owned(),
        window_width: WINDOW_W as i32,
        window_height: WINDOW_H as i32,
        high_dpi: true,
        fullscreen: false,
        window_resizable: false,
        sample_count: Default::default(),
        icon: None,
        platform: Default::default()
    }
}

#[macroquad::main(window_conf)]
async fn main() {
    // instantiate game
    let mut gol = GameOfLife::new();

    let mut update_frame_cap = 0.75;
    let mut passed_time: f32 = 0.0;

    loop {
        if passed_time >= update_frame_cap {
            passed_time = 0.0;
        }
        passed_time += get_frame_time();


        gol.run(&mut passed_time, &mut update_frame_cap);

        next_frame().await;
    }
    
}
