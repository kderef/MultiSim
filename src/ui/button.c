#ifndef BUTTON_C_
#define BUTTON_C_

#include "raylib.h"
#include "raymath.h"
#include "font.c"

//! Simple wrapper code to draw buttons to the screen

#define UI_BUTTONS_TEXT_COLOR BLACK
#define UI_BUTTONS_HEIGHT 50
#define UI_BUTTONS_CORNER_RADIUS 0.1
#define UI_BUTTONS_CORNER_SEGMENTS 10
#define UI_BUTTONS_FONT_SIZE 40.0
#define UI_BUTTONS_TEXT_Y_OFFSET 5

// macro for reducing repeated code
#define BUTTON_DRAW(BTN, MOUSE_POS, SELECTED) \
    if (button_is_hovered(&(BTN), &(MOUSE_POS))) { \
        if (mouse_clicked) { \
            button_draw(&(BTN)); \
            return SELECTED; \
        } \
        button_draw_hovered(&(BTN)); \
    } else button_draw(&(BTN));

typedef struct {
    const char* text;
    int text_x;
    Rectangle body;
} Button;

Button button_new(const char* text, int text_offset, int x, int y, int width, int height) {
    return (Button) {
        text, text_offset, (Rectangle){x, y, width, height}
    };
}

void button_draw(Button* btn) {
    DrawRectangleRounded(
        btn->body,
        UI_BUTTONS_CORNER_RADIUS,
        UI_BUTTONS_CORNER_SEGMENTS,
        LIGHTGRAY
    );

    DrawTextD(
        btn->text, btn->text_x,
        btn->body.y + UI_BUTTONS_TEXT_Y_OFFSET,
        UI_BUTTONS_FONT_SIZE,
        UI_BUTTONS_TEXT_COLOR
    );
}

void button_draw_hovered(Button* btn) {
    DrawRectangleRounded(
        btn->body,
        UI_BUTTONS_CORNER_RADIUS,
        UI_BUTTONS_CORNER_SEGMENTS,
        LIGHTGRAY
    );
    DrawRectangleRoundedLines(
        btn->body,
        UI_BUTTONS_CORNER_RADIUS,
        UI_BUTTONS_CORNER_SEGMENTS,
        2.0,
        RAYWHITE
    );

    DrawTextD(
        btn->text, btn->text_x,
        btn->body.y + UI_BUTTONS_TEXT_Y_OFFSET,
        UI_BUTTONS_FONT_SIZE,
        UI_BUTTONS_TEXT_COLOR
    );
}

void button_draw_clicked(Button* btn) {
        DrawRectangleRounded(
        btn->body,
        UI_BUTTONS_CORNER_RADIUS,
        UI_BUTTONS_CORNER_SEGMENTS,
        GRAY
    );
    DrawRectangleRoundedLines(
        btn->body,
        UI_BUTTONS_CORNER_RADIUS,
        UI_BUTTONS_CORNER_SEGMENTS,
        2.0,
        RAYWHITE
    );

    DrawTextD(
        btn->text, btn->text_x,
        btn->body.y + UI_BUTTONS_TEXT_Y_OFFSET,
        UI_BUTTONS_FONT_SIZE,
        UI_BUTTONS_TEXT_COLOR
    );
}

bool button_is_hovered(Button* btn, Vector2* mouse_pos) {
    return CheckCollisionPointRec(
        *mouse_pos,
        btn->body
    );
}

#endif