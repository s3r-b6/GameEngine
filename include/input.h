#pragma once

#include <map>

#include "./types.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_log.h"

struct KeyState {
    bool isDown;
    bool wasDown;
    bool justPressed;
    bool justReleased;
};

struct Input {
    glm::ivec2 mouseWorldPos;
    glm::ivec2 mousePos;

    bool mouseInWindow;
    bool showCursor;

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
