use rust_raylib::{
    math::Vector2 as Vec2,
    texture::Texture2D,
    ffi
};

use crate::selector::SelectedGame;

const DVD_LOGO_BYTES: &[u8] = include_bytes!("../assets/DVD_logo.png");

pub struct Dvd {
    pos: Vec2,
    velocity: Vec2,
    window_size: Vec2,
    logo: Texture2D,
    logo_size: Vec2,
}

impl crate::game::Game for Dvd {
    fn new() -> Self {
        let window_size = Vec2{x};

        let logo_raw = unsafe {
            let img = ffi::LoadImageFromMemory(
                ".png".as_ptr() as *const i8, DVD_LOGO_BYTES.as_ptr(),
                DVD_LOGO_BYTES.len() as i32
            );
            let l = ffi::LoadTextureFromImage(img);
            ffi::UnloadImage(img);
            l
        };

        let logo = unsafe { Texture2D::from_raw(logo_raw) };
        let logo_size = Vec2 { x: logo.width() as f32, y: logo.height() as f32 };

        let rand_x = rand::gen_range(0.0, window_size.x - logo_size.x);
        let rand_y = rand::gen_range(0.0, window_size.y - logo_size.y);

        Self {
            pos: Vec2{x: rand_x, y: rand_y},
            velocity: vec2(180.0, 180.0),
            window_size,
            logo_size,
            logo,
        }
    }

    fn update(&mut self) -> SelectedGame {
        let next = if let Some(KeyCode::Escape) = get_last_key_pressed() {
            SelectedGame::None
        } else {
            SelectedGame::Dvd
        };

        let dt = get_frame_time();

        self.window_size.x = screen_width();
        self.window_size.y = screen_height();

        self.pos = self
            .pos
            .clamp(vec2(0., 0.), self.window_size - self.logo_size)
            + self.velocity * dt;

        if self.pos.x + self.logo_size.x >= self.window_size.x || self.pos.x <= 0.0 {
            self.velocity.x *= -1.0;
        }
        if self.pos.y + self.logo_size.y >= self.window_size.y || self.pos.y <= 0.0 {
            self.velocity.y *= -1.0;
        }

        clear_background(WHITE);
        draw_texture(
            &self.logo,
            self.pos.x,
            self.pos.y,
            WHITE
        );

        next
    }
}
