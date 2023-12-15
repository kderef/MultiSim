#[repr(u8)]
#[derive(Clone, Copy, Debug)]
pub enum Cell {
    Dead = 0,
    Alive = 1,
}
pub type Cells = Vec<Cell>;

impl Cell {
    pub fn calculate_next_iteration(&self, neighbours: usize) -> Self {
        match self {
            Self::Alive => match neighbours {
                2..=3 => Self::Alive, // survival
                _ => Self::Dead,      // under- or overpopulation
            },
            Self::Dead => {
                if neighbours == 3 {
                    Self::Alive // reproduction
                } else {
                    Self::Dead // remain dead
                }
            }
        }
    }
}
