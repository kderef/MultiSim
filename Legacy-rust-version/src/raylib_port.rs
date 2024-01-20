use std::{ffi::{CStr, CString}, ops::{Add, Div, Mul, Sub}};
pub use raylib_ffi::enums::KeyboardKey;

pub struct Texture2D(raylib_ffi::Texture2D);
pub struct Image(raylib_ffi::Image);
pub struct Font(raylib_ffi::Font);

/* Memory drops */
impl Drop for Texture2D {
    fn drop(&mut self) {
        unsafe { raylib_ffi::UnloadTexture(self.0) }
    }
}

impl Drop for Image {
    fn drop(&mut self) {
        unsafe { raylib_ffi::UnloadImage(self.0) }
    }
}

impl Drop for Font {
    fn drop(&mut self) {
        unsafe { raylib_ffi::UnloadFont(self.0) }
    }
}

/* Impls */
impl Texture2D {
    pub fn from_image(img: Image) -> Self {
        Self(unsafe {
            raylib_ffi::LoadTextureFromImage(img.0)
        })
    }
    pub fn size(&self) -> Vector2 {
        Vector2::new(
            self.0.width as f32,
            self.0.height as f32
        )
    }
    pub fn draw(&self, x: i32, y: i32, color: raylib_ffi::Color) {
        unsafe {
            raylib_ffi::DrawTexture(
                self.0, x, y, color
            );
        }
    }
}

impl Image {
    pub fn from_bytes(filetype: &str, data: &[u8]) -> Self {
        let ftype = CString::new(filetype).unwrap();
        Self(
            unsafe {
                raylib_ffi::LoadImageFromMemory(
                    ftype.as_ptr(),
                    data.as_ptr(), data.len() as i32
                )
            }
        )
    }
}

impl Font {
    pub fn from_bytes(filetype: &str, data: &[u8]) -> Self {
        let ftype = CString::new(filetype).unwrap();
        Self(
            unsafe {
                raylib_ffi::LoadFontFromMemory(
                    ftype.as_ptr(), data.as_ptr(),
                    data.len() as i32, 100, std::ptr::null_mut(), 0)
            }
        )
    }
}

/********************************************************/
// random

pub fn seed_random(seed: u32) {
    unsafe {
        raylib_ffi::SetRandomSeed(seed);
    }
}

pub fn get_random(min: i32, max: i32) -> i32 {
    unsafe {
        raylib_ffi::GetRandomValue(min, max)
    }
}

/********************************************************/
// Keys

pub fn get_last_key_pressed() -> Option<KeyboardKey> {
    unsafe {
        match raylib_ffi::GetKeyPressed() {
            0 => None,
            key => Some(std::mem::transmute(key))
        }
    }
}


/********************************************************/
// Math


#[repr(C)]
#[derive(Debug, Clone, Copy, Default, PartialEq, PartialOrd)]
pub struct Vector2 {
    pub x: f32,
    pub y: f32
}

#[repr(C)]
#[derive(Debug, Clone, Copy, Default, PartialEq, PartialOrd)]
pub struct Vector3 {
    pub x: f32,
    pub y: f32,
    pub z: f32
}

#[repr(C)]
#[derive(Debug, Clone, Copy, Default, PartialEq, PartialOrd)]
pub struct Rectangle {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32
}

impl Vector2 {
    pub const fn new(x: f32, y: f32) -> Self {
        Self {x, y}
    }
    pub fn clamp(&self, min: Self, max: Self) -> Self {
        Self {
            x: self.x.clamp(min.x, max.x),
            y: self.y.clamp(min.y, max.y)
        }
    }
}

impl Add for Vector2 {
    type Output = Self;
    fn add(self, rhs: Self) -> Self::Output {
        Self {
            x: self.x + rhs.x,
            y: self.y + rhs.y
        }
    }
}

impl Sub for Vector2 {
    type Output = Self;
    fn sub(self, rhs: Self) -> Self::Output {
        Self {
            x: self.x - rhs.x,
            y: self.y - rhs.y
        }
    }
}

impl Mul for Vector2 {
    type Output = Self;
    fn mul(self, rhs: Self) -> Self::Output {
        Self {
            x: self.x * rhs.x,
            y: self.y * rhs.y
        }
    }
}

impl Mul<f32> for Vector2 {
    type Output = Self;
    fn mul(self, rhs: f32) -> Self::Output {
        Self {
            x: self.x * rhs,
            y: self.y * rhs
        }
    }
}

impl Add for Vector3 {
    type Output = Self;
    fn add(self, rhs: Self) -> Self::Output {
        Self {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
            z: self.z + rhs.z
        }
    }
}

impl Vector3 {
    pub const fn new(x: f32, y: f32, z: f32) -> Self {
        Self {x, y, z}
    }
}

impl Rectangle {
    pub const fn new(x: f32, y: f32, width: f32, height: f32) -> Self {
        Self{x, y, width, height}
    }
    pub fn draw(&self, color: raylib_ffi::Color) {
        unsafe {
            raylib_ffi::DrawRectangleRec(std::mem::transmute(*self), color);
        }
    }
}

pub const VEC2_ZERO: Vector2 = Vector2 {x: 0.0, y: 0.0};
pub const VEC3_ZERO: Vector3 = Vector3 {x: 0.0, y: 0.0, z: 0.0};
pub const RECTANGLE_ZERO: Rectangle = Rectangle {x: 0.0, y: 0.0, width: 0.0, height: 0.0};

pub fn get_frame_time() -> f32 {
    unsafe {
        raylib_ffi::GetFrameTime()
    }
}

/********************************************************/
// global functions

pub fn begin_drawing() {
    unsafe {
        raylib_ffi::BeginDrawing();
    }
}

pub fn end_drawing() {
    unsafe {
        raylib_ffi::EndDrawing();
    }
}

pub fn clear_background(color: raylib_ffi::Color) {
    unsafe {
        raylib_ffi::ClearBackground(color);
    }
}

pub fn screen_width() -> i32 {
    unsafe {
        raylib_ffi::GetScreenWidth()
    }
}

pub fn screen_height() -> i32 {
    unsafe {
        raylib_ffi::GetScreenHeight()
    }
}

/********************************************************/
// window struct

pub struct Window {
    width: i32, height: i32,
    min_w: i32, min_h: i32,
    title: &'static str,
    target_fps: u32,
}



impl Window {
    pub const fn init(width: i32, height: i32, title: &'static str) -> Self {
        Self {
            width, height, title,
            target_fps: 60,
            min_w: width, min_h: height
        }
    }
    pub fn set_target_fps(&mut self, target: u32) {
        self.target_fps = target;
    }
    pub fn show(&self) {
        let title_ = CString::new(self.title).unwrap();

        unsafe {
            raylib_ffi::InitWindow(self.width, self.height, title_.as_ptr());
            raylib_ffi::SetTargetFPS(self.target_fps as i32);
        }
    }
    pub fn should_close(&self) -> bool {
        unsafe {
            raylib_ffi::WindowShouldClose()
        }
    }
    pub fn set_icon(&mut self, icon: Image) {
        unsafe {
            raylib_ffi::SetWindowIcon(icon.0);
        }
    }
    pub fn set_exit_key(&mut self, key: KeyboardKey) {
        unsafe {
            raylib_ffi::SetExitKey(key as i32);
        }
    }
    pub fn set_min_size(&mut self, width: i32, height: i32) {
        self.min_w = width;
        self.min_h = height;
    }
}

impl Drop for Window {
    fn drop(&mut self) {
        unsafe {
            raylib_ffi::CloseWindow();
        }
    }
}

pub fn init_audio_device() {
    unsafe {
        raylib_ffi::InitAudioDevice();
    }
}