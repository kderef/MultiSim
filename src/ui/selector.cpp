#ifndef SELECTOR_C_
#define SELECTOR_C_

#include "raygui_incl.h"
#include "raygui_style_dark.h"
#include "../const.hpp"
#include "../gol/game.cpp"
#include "../gamestate.hpp"
#include "../dvd/dvd.cpp"
#include "../pong/pong.cpp"
#include <time.h>

struct Selector {
private:
// games
    GameOfLife gol;
    Dvd dvd;
    Pong pong;
    
    int button_x;
    int screen_x_center;
    int screen_y_center;

    SelectedGame next_game;

public:
    SelectedGame selected;
    GameState state;

    Selector() :
        gol(),
        dvd(),
        pong(),
        selected(Selected_None)
    {
        GuiLoadStyleDark();
    }

    SelectedGame title_screen() {
        constexpr int BUTTONS_SPACING = 80;
        constexpr int BUTTON_HEIGHT = 50;

        global_screen_width = GetScreenWidth();
        global_screen_height = GetScreenHeight();

        screen_x_center = global_screen_width / 2;
        screen_y_center = global_screen_height / 2;

        if (IsKeyPressed(KEY_F5)) global_show_fps = !global_show_fps;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextD(
            "MultiSim",
            screen_x_center - 145,
            50,
            100.0, RAYWHITE
        );

        GuiDrawText("By Kian (kn-ht)", rect(0, global_screen_height - 20, 200, 20), TEXT_ALIGN_LEFT, GRAY);

        // draw the buttons
        button_x = screen_x_center * 0.5;

        if (GuiButton(rect(button_x, screen_y_center - BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "Game of Life")) {
            return Selected_GOL;
        }
        if (GuiButton(rect(button_x, screen_y_center, screen_x_center, BUTTON_HEIGHT), "DvD Bouncy")) {
            return Selected_DVD;
        }
        if (GuiButton(rect(button_x, screen_y_center + BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "Pong")) {
            return Selected_PONG;
        }

        return Selected_None;
    }

    void update() {
        global_screen_width = GetScreenWidth();
        global_screen_height = GetScreenHeight();

        switch (selected) {
            case Selected_None: {
                next_game = this->title_screen();
            } break;
            case Selected_GOL: {
                next_game = gol.update();
            } break;
            case Selected_DVD: {
                next_game = dvd.update();
            } break;
            case Selected_PONG: {
                next_game = pong.update();
            } break;
        }

        if (global_show_fps) DrawTextD(TextFormat("FPS: %d", GetFPS()), global_screen_width - 130, 0, 33.0, GOLD);
        EndDrawing();

        if (next_game != selected) {
            SetWindowTitle(selected_get_window_title(next_game));
            selected = next_game;
        }
    }
};

#endif