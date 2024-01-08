#ifndef DVD_C_
#define DVD_C_

#include "raylib.h"
#include "raymath.h"
#include "../ui/raygui_incl.h"
#include "../gamestate.hpp"
#include "../const.hpp"
#include "../ui/font.c"

//! All of the code for rendering and handling the DvD 'game'.

struct Dvd {
private:
    GameState state;

    Vector2 position;
    Vector2 velocity;
    Vector2 logo_size;
    bool inverted, rainbow;
    float passed_time;

    float spacing;
    float dt;
    int key;

public:
    Dvd() {
        state = GameState_Help;
        logo_size = vec2(dvd_logo_texture.width, dvd_logo_texture.height);
        velocity = vec2(250, 250);
        inverted = true;
        rainbow = true;
        position = vec2(
            GetRandomValue(0, GetScreenWidth() - dvd_logo_texture.width),
            GetRandomValue(0, GetScreenHeight() - dvd_logo_texture.height)
        );
    }

    void set_random_pos() {
        position = vec2(
            GetRandomValue(0, global_screen_width - dvd_logo_texture.width),
            GetRandomValue(0, global_screen_height - dvd_logo_texture.height)
        );
    }

    void draw_help() {
        spacing = 0.0f;
        
        BeginDrawing();
        ClearBackground(BLACK);

        GuiButton(
            rect(50, 50, 300, 300), "Hello World!"
        );

        DrawTextD("DvD controls", 3, 0, FONT_XL, GREEN);
        DrawTextD("H         - toggle help mode", 3, spacing += FONT_XL, FONT_M, RAYWHITE);
        DrawTextD("I          - invert the colors", 3, spacing += FONT_M, FONT_M, RAYWHITE);
        DrawTextD("R         - randomize the DvD logo position", 3, spacing += FONT_M, FONT_M, RAYWHITE);
        DrawTextD("C         - toggle rainbow colors", 3, spacing += FONT_M, FONT_M, RAYWHITE);
        DrawTextD("Space   - pause/unpause the DvD logo", 3, spacing += FONT_M, FONT_M, RAYWHITE);
    }

    SelectedGame update() {
        key = GetKeyPressed();
        switch (key) {
            case KEY_ESCAPE: {
                state = GameState_Paused;
                return Selected_None;   
            }
            case KEY_H: {
                state = (state == GameState_Help)? GameState_Paused : GameState_Help;
            } break;
            case KEY_I: {
                inverted = !(inverted);
            } break;
            case KEY_R: {
                this->set_random_pos();
            } break;
            case KEY_C: {
                rainbow = !(rainbow);
            } break;
            case KEY_SPACE: {
                state = (state == GameState_Running)? GameState_Paused : GameState_Running;
            } break;
            case KEY_F5: {
                global_show_fps = !global_show_fps;
            } break;
            default: {}
        }
        
        dt = GetFrameTime();
        passed_time = (passed_time > 36.0f)? 0.0f : passed_time + dt;

        switch (state) {
            case GameState_Help: 
                this->draw_help();
                return Selected_DVD;
            case GameState_Running: goto update_dvd;
            case GameState_Paused: goto draw_dvd;
            default: {}
        }

    update_dvd:
        position = Vector2Add(
            Vector2Multiply(
                (Vector2){dt, dt},
                velocity
            ),
            Vector2Clamp(
                position,
                (Vector2){0, 0},
                Vector2Subtract(
                    vec2(global_screen_width, global_screen_height),
                    logo_size
                )
            )
        );

        if (
            position.x + dvd_logo_texture.width >= global_screen_width ||
            position.x <= 0
        ) velocity.x *= -1;
        
        if (position.y + dvd_logo_texture.height >= global_screen_height ||
            position.y <= 0
        ) velocity.y *= -1;

    draw_dvd:
        static Color tint;
        tint = rainbow?
            inverted?
                ColorFromHSV(passed_time * 10.0f, 1.0, 1.0) :
                ColorFromHSV(passed_time * 10.0, 0.6, 1.0)
            : inverted? WHITE : BLACK;

        BeginDrawing();
        ClearBackground((inverted)? BLACK : RAYWHITE);
        if (state == GameState_Paused) {
            DrawTextD("[SPACE TO UNPAUSE]", 3, global_screen_height - FONT_S, FONT_S, GREEN);
        }
        DrawTexture(
            dvd_logo_texture,
            position.x,
            position.y,
            tint
        );

        return Selected_DVD;
    }
};

#endif