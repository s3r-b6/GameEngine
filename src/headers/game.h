#pragma once

#include "./input.h"
#include "./platform.h"
#include "./renderer.h"

constexpr glm::ivec2 WORLD_SIZE = {320, 180};
constexpr int TILESIZE = 16;

enum GameAction {
    UNKNOWN,

    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    QUIT,

    GAME_ACTION_COUNT,
};

struct GameState {
    bool initialized;
    double updateTimer;
    glm::ivec2 playerPos;
    SDL_Keycode gameBinds[GAME_ACTION_COUNT] = {0};
};

global GameState *gGameState;

inline bool gameRegisterKey(GameAction action, SDL_Keycode kc) {
    gGameState->gameBinds[action] = kc;
    if (registerKey(kc)) {
        SDL_Log("Succesfuly bound %c to game action: %d", kc, action);
        return true;
    } else {
        SDL_Log("Could not bind %c to game action: %d", kc, action);
        return false;
    }
}

inline bool actionDown(GameAction action) {
    if (!gGameState->gameBinds[action]) return false;

    KeyState ks = {};
    getKeyState(gGameState->gameBinds[action], &ks);

    return ks.isDown;
}

inline bool actionUp(GameAction action) {
    if (!gGameState->gameBinds[action]) return false;

    KeyState ks = {};
    getKeyState(gGameState->gameBinds[action], &ks);

    return !ks.isDown;
}

// TODO: In win32 we need __declspec(dllexport)
extern "C" {
EXPORT_FN void updateGame(GameState *gameStateIn, RenderData *renderDataIn,
                          Input *inputIn, ImguiState *gImgui, float dt);
}
