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

    inline bool lMouseJustPressed() { return mouseInWindow && mouseState[0] && !prevMouseState[0]; }

    inline bool rMouseJustPressed() { return mouseInWindow && mouseState[1] && !prevMouseState[1]; }
    inline bool mMouseJustPressed() { return mouseInWindow && mouseState[2] && !prevMouseState[2]; }

    inline bool lMouseDown() { return mouseInWindow && mouseState[0]; }
    inline bool rMouseDown() { return mouseInWindow && mouseState[1]; }
    inline bool mMouseDown() { return mouseInWindow && mouseState[2]; }

    inline bool lMouseUp() { return mouseInWindow && !mouseState[0]; }
    inline bool rMouseUp() { return mouseInWindow && !mouseState[1]; }
    inline bool mMouseUp() { return mouseInWindow && !mouseState[2]; }

    inline bool keyIsUp(SDL_Keycode kc) { return !keyboardState[SDL_GetScancodeFromKey(kc)]; }
    inline bool keyIsDown(SDL_Keycode kc) { return keyboardState[SDL_GetScancodeFromKey(kc)]; }
};

// This should return true only once until the action key is released
inline bool actionJustPressed(GameState *gameState, Input *input, GameAction action) {
    bool keyPressed = input->keyboardState[gameState->gameBinds[action]];
    if (!keyPressed || gameState->actionsTriggered[action]) { return false; }
    gameState->actionsTriggered[action] = true;
    return true;
}

// This returns true until the action key is released
inline bool actionDown(GameState *gameState, Input *input, GameAction action) {
    return input->keyboardState[gameState->gameBinds[action]];
}

// This returns true while the action key is up
inline bool actionUp(GameState *gameState, Input *input, GameAction action) {
    return !input->keyboardState[gameState->gameBinds[action]];
}

// This returns true only once until the action key is pressed
inline bool actionJustReleased(GameState *gameState, Input *input, GameAction action) {
    bool keyReleased = !input->keyboardState[gameState->gameBinds[action]];

    return keyReleased && gameState->actionsTriggered[action];
}

inline void releaseActions(GameState *gameState, Input *input) {
    for (int i = 0; i < GAME_ACTION_COUNT; i++) {
        bool keyReleased = !input->keyboardState[gameState->gameBinds[i]];
        if (keyReleased) { gameState->actionsTriggered[i] = false; }
    }
}
