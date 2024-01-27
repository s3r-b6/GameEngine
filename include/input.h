#pragma once

#include <map>

#include "./game.h"
#include "./types.h"

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
    glm::ivec2 mousePos;

    bool mouseInWindow;
    bool showCursor;

    // TODO: This should not be like this
    bool mLeftDown;
    bool mRightDown;
    bool mMidDown;

    SDL_Keycode lastPressed;

    std::map<SDL_Keycode, KeyState> usedKeys;
};

inline bool unregisterKey(SDL_Keycode kc, Input *input) {
    auto keyPair = input->usedKeys.find(kc);
    if (keyPair != input->usedKeys.end()) {
        // SDL_Log("Keycode %d already found in the keys map", kc);
        return false;
    }

    input->usedKeys.erase(keyPair);
    return true;
}

inline bool registerKey(SDL_Keycode kc, Input *input) {
    auto keyPair = input->usedKeys.find(kc);

    if (keyPair != input->usedKeys.end()) {
        // SDL_Log("Keycode %d already found in the keys map", kc);
        return false;
    }

    SDL_Log("Keycode %d not found in the keys map, registering it", kc);
    KeyState ks = {0};
    input->usedKeys.insert({kc, ks});
    return true;
}

inline bool updateKeyState(SDL_Keycode kc, bool pressed, Input *input) {
    auto keyPair = input->usedKeys.find(kc);

    // If it does not find the key, it returns the post end iterator
    if (keyPair == input->usedKeys.end()) {
        // SDL_Log("Keycode %d not found in the keys map", kc);
        return false;
    } else {
        // SDL_Log("Keycode %d found in the keys map", kc);

        keyPair->second.justPressed = pressed & !keyPair->second.wasDown;
        keyPair->second.justReleased = !pressed && keyPair->second.wasDown;
        keyPair->second.wasDown = keyPair->second.isDown;
        keyPair->second.isDown = pressed;

        return true;
    }
}

inline bool getKeyState(SDL_Keycode kc, KeyState *state, Input *input) {
    auto keyPair = input->usedKeys.find(kc);

    if (keyPair == input->usedKeys.end()) {
        SDL_Log("Keycode %d not found in the keys map", kc);
        return false;
    }

    // SDL_Log("Keycode %d found in the keys map", kc);
    *state = keyPair->second;
    return true;
}

// TODO: Implement some kind of InputManager
inline bool gameRegisterKey(GameState *gameState, Input *input, GameAction action, SDL_Keycode kc) {
    gameState->gameBinds[action] = kc;
    if (registerKey(kc, input)) {
        SDL_Log("Succesfuly bound %c to game action: %d", kc, action);
        return true;
    } else {
        SDL_Log("Could not bind %c to game action: %d", kc, action);
        return false;
    }
}

inline bool actionDown(GameState *gameState, Input *input, GameAction action) {
    if (!gameState->gameBinds[action]) return false;

    KeyState ks = {0};
    getKeyState(gameState->gameBinds[action], &ks, input);

    return ks.isDown;
}

inline bool actionUp(GameState *gameState, Input *input, GameAction action) {
    if (!gameState->gameBinds[action]) return false;

    KeyState ks = {0};
    getKeyState(gameState->gameBinds[action], &ks, input);

    return !ks.isDown;
}
