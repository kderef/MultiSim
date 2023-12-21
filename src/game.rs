pub trait Game {
    fn new() -> Self;
    fn update(&mut self) -> crate::selector::SelectedGame;
}