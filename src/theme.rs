use macroquad::{
    color::{BLACK, BLUE, GREEN, WHITE},
    prelude::Color,
};

#[derive(Debug, Clone, Copy)]
pub struct Style {
    bg: Color,
    fg: Color,
    accent: Color,
}
impl Style {
    pub const fn default() -> Self {
        Self {
            bg: BLACK,
            fg: WHITE,
            accent: GREEN,
        }
    }
    pub const fn new(bg: Color, fg: Color, accent: Color) -> Self {
        Self { bg, fg, accent }
    }
}
impl Into<(Color, Color, Color)> for Style {
    fn into(self) -> (Color, Color, Color) {
        (self.bg, self.fg, self.accent)
    }
}

#[derive(Clone, Debug)]
pub enum Theme {
    Default = 0,
    Gruvbox,
    Matrix,
    Midnight,
}

macro_rules! hex_color {
    ($hex:literal) => {
        macroquad::color::Color {
            r: (($hex >> 16) & 0xFF) as f32 / 255.,
            g: (($hex >> 8) & 0xFF) as f32 / 255.,
            b: ($hex & 0xFF) as f32 / 255.,
            a: 1.0,
        }
    };
}

static GRUVBOX: Style = Style::new(hex_color!(0x282828), hex_color!(0xebdbb2), hex_color!(0xcc241d));
static MATRIX: Style = Style::new(hex_color!(0x131721), hex_color!(0x32c603), BLUE);
static MIDNIGHT: Style = Style::new(hex_color!(0x0D1017), WHITE, hex_color!(0xd65d0e));

impl Theme {
    pub fn to_style(&self) -> Style {
        match *self {
            Self::Default => Style::default(),
            Self::Gruvbox => GRUVBOX,
            Self::Matrix => MATRIX,
            Self::Midnight => MIDNIGHT,
        }
    }
    pub fn cycle(&mut self) {
        *self = match *self {
            Self::Default => Self::Gruvbox,
            Self::Gruvbox => Self::Matrix,
            Self::Matrix => Self::Midnight,
            Self::Midnight => Self::Default,
        };
    }
}
