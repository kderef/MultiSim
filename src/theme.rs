use std::{iter::Cycle, slice::Iter};

use macroquad::{
    color::{BLACK, WHITE, GREEN, BLUE},
    prelude::Color,
};

#[derive(Debug)]
pub struct Style {
    bg: Color,
    fg: Color,
    accent: Color,
}
impl Style {
    pub fn default() -> Self {
        Self {
            bg: BLACK,
            fg: WHITE,
            accent: GREEN,
        }
    }
}
impl Into<(Color, Color, Color)> for Style {
    fn into(self) -> (Color, Color, Color) {
        (self.bg, self.fg, self.accent)
    }
}

#[derive(Clone, Debug)]
pub enum Theme {
    Default = 0, Gruvbox, Matrix, Midnight
}

impl Theme {
    pub fn to_style(&self) -> Style {
        match *self {
            Self::Default => Style::default(),
            Self::Gruvbox => Style {
                bg: Color::from_hex(0x282828),
                fg: Color::from_hex(0xebdbb2),
                accent: Color::from_hex(0xcc241d)
            },
            Self::Matrix => Style {
                bg: Color::from_hex(0x131721),
                fg: Color::from_hex(0x32c603),
                accent: BLUE,
            },
            Self::Midnight => Style {
                bg: Color::from_hex(0x0D1017),
                fg: WHITE,
                accent: Color::from_hex(0xd65d0e)
            }
        }
    }
    pub fn cycle(&mut self) {
        *self = match *self {
            Self::Default => Self::Gruvbox,
            Self::Gruvbox => Self::Matrix,
            Self::Matrix => Self::Midnight,
            Self::Midnight => Self::Default
        };
    }
}