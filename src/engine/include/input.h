#pragma once

#include <map>

#include "game.h"
#include "types.h"

#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_log.h"

struct KeyState {
    bool isDown;
    bool wasDown;
    bool justPressed;
    bool justReleased;
};

// TODO: The input system should allow also controllers
struct Input {
    glm::ivec2 mouseWorldPos;
    glm::ivec2 mouseUiPos;
    glm::ivec2 mousePos;

    bool mouseInWindow;
    bool showCursor;

    u8 mouseState = 0;

    // SDL_Keycode lastPressed;
    const u8 *keyboardState;
    u8 *previousKeyboardState;

    void debug_printMouseState() {
        for (int i = 7; i >= 0; i--) {
            printf("%d", (mouseState >> i) & 1);
        }
        printf("\n");
    }

    bool lMouseJustPressed() {
        bool left_down = (mouseState & 0x01) != 0;
        bool was_down = (mouseState & (0x01 << 4)) != 0;

        return mouseInWindow && left_down && !was_down;
    }

    bool mMouseJustPressed() {
        bool mid_down = (mouseState & (0x01 << 1)) != 0;
        bool was_down = (mouseState & (0x01 << 5)) != 0;

        return mouseInWindow && mid_down && !was_down;
    }

    bool rMouseJustPressed() {
        bool right_down = (mouseState & (0x01 << 2)) != 0;
        bool was_down = (mouseState & (0x01 << 6)) != 0;

        // if (right_down && !was_down) { debug_printMouseState(); }
        return mouseInWindow && right_down && !was_down;
    }

    bool lMouseJustReleased() {
        bool left_down = (mouseState & 0x01) != 0;
        bool was_down = (mouseState & (0x01 << 4)) != 0;

        return mouseInWindow && !left_down && was_down;
    }

    bool mMouseJustReleased() {
        bool mid_down = (mouseState & (0x01 << 1)) != 0;
        bool was_down = (mouseState & (0x01 << 5)) != 0;

        return mouseInWindow && !mid_down && was_down;
    }
    bool rMouseJustReleased() {
        bool right_down = (mouseState & (0x01 << 2)) != 0;
        bool was_down = (mouseState & (0x01 << 6)) != 0;

        // if (!right_down && was_down) { debug_printMouseState(); }
        return mouseInWindow && !right_down && was_down;
    }

    bool lMouseDown() {
        bool left_down = (mouseState & 0x01) != 0;
        return mouseInWindow && left_down;
    }
    bool mMouseDown() {
        bool mid_down = (mouseState & (0x01 << 1)) != 0;
        return mouseInWindow && mid_down;
    }
    bool rMouseDown() {
        bool right_down = (mouseState & (0x01 << 2)) != 0;

        // if (right_down) { debug_printMouseState(); }
        return mouseInWindow && right_down;
    }

    bool lMouseUp() {
        bool left_down = (mouseState & 0x01) != 0;
        return mouseInWindow && !left_down;
    }
    bool mMouseUp() {
        bool mid_down = (mouseState & (0x01 << 1)) != 0;
        return mouseInWindow && !mid_down;
    }
    bool rMouseUp() {
        bool right_down = (mouseState & (0x01 << 2)) != 0;
        return mouseInWindow && !right_down;
    }

    bool keyIsUp(SDL_Keycode kc) { return !keyboardState[SDL_GetScancodeFromKey(kc)]; }
    bool keyIsDown(SDL_Keycode kc) { return keyboardState[SDL_GetScancodeFromKey(kc)]; }
    bool keyJustPressed(SDL_Keycode kc) {
        bool currentState = keyboardState[SDL_GetScancodeFromKey(kc)];
        bool previousState = previousKeyboardState[SDL_GetScancodeFromKey(kc)];

        previousKeyboardState[SDL_GetScancodeFromKey(kc)] = currentState;

        return currentState && !previousState;
    }
};
