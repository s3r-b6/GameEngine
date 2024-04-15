#pragma once

#include <map>

#include "./game.h"
#include "./types.h"

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
    glm::ivec2 mouseUIpos;
    glm::ivec2 mousePos;

    bool mouseInWindow;
    bool showCursor;

    // TODO: This should not be like this
    bool mouseState[3] = {0};
    bool prevMouseState[3] = {0};

    SDL_Keycode lastPressed;
    const u8 *keyboardState;
    u8 *previousKeyboardState;

    inline bool lMouseJustPressed() { return mouseInWindow && mouseState[0] && !prevMouseState[0]; }
    inline bool lMouseJustReleased() {
        return mouseInWindow && !mouseState[0] && prevMouseState[0];
    }

    inline bool mMouseJustPressed() { return mouseInWindow && mouseState[1] && !prevMouseState[1]; }
    inline bool rMouseJustPressed() { return mouseInWindow && mouseState[2] && !prevMouseState[2]; }

    inline bool lMouseDown() { return mouseInWindow && mouseState[0]; }
    inline bool mMouseDown() { return mouseInWindow && mouseState[1]; }
    inline bool rMouseDown() { return mouseInWindow && mouseState[2]; }

    inline bool lMouseUp() { return mouseInWindow && !mouseState[0]; }
    inline bool mMouseUp() { return mouseInWindow && !mouseState[1]; }
    inline bool rMouseUp() { return mouseInWindow && !mouseState[2]; }

    inline bool keyIsUp(SDL_Keycode kc) { return !keyboardState[SDL_GetScancodeFromKey(kc)]; }
    inline bool keyIsDown(SDL_Keycode kc) { return keyboardState[SDL_GetScancodeFromKey(kc)]; }
    bool keyJustPressed(SDL_Keycode kc) {
        bool currentState = keyboardState[SDL_GetScancodeFromKey(kc)];
        bool previousState = previousKeyboardState[SDL_GetScancodeFromKey(kc)];

        previousKeyboardState[SDL_GetScancodeFromKey(kc)] = currentState;

        return currentState && !previousState;
    }
};