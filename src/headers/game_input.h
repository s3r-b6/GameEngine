#pragma once

#include "SDL2/SDL_keycode.h"
#include "input.h"

enum GameAction {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    QUIT,

    GAME_ACTION_COUNT,
};

SDL_Keycode gameBinds[GAME_ACTION_COUNT] = {0};

bool gameRegisterKey(GameAction action, SDL_Keycode kc) {
    gameBinds[action] = kc;
    if (registerKey(kc)) {
        SDL_Log("Succesfuly bound %c to game action: %d", kc, action);
        return true;
    } else {
        SDL_Log("Could not bind %c to game action: %d", kc, action);
        return false;
    }
}

bool actionDown(GameAction action) {
    if (!gameBinds[action]) return false;

    KeyState ks = {};
    getKeyState(gameBinds[action], &ks);

    return ks.isDown;
}

bool actionUp(GameAction action) {
    if (!gameBinds[action]) return false;

    KeyState ks = {};
    getKeyState(gameBinds[action], &ks);

    return !ks.isDown;
}
