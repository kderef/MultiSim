#ifndef SELECTEDGAME_H_
#define SELECTEDGAME_H_

typedef enum {
    Selected_None = 0,
    Selected_GOL,
    Selected_DVD,
} SelectedGame;

static const char* SELECTED_TITLES[] = {
    [Selected_None] = "MultiSim - menu",
    [Selected_GOL] = "MultiSim - Game of Life",
    [Selected_DVD] = "MultiSim - DvD bouncy"
};

#define selected_get_window_title(S) (SELECTED_TITLES[S])

#endif