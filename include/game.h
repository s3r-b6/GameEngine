#pragma once

#include "./platform.h"
#include "./types.h"

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

extern "C" {
EXPORT_FN void updateGame(GlobalState *globalStateIn, float dt);
}

inline bool gameRegisterKey(GameAction action, SDL_Keycode kc);
inline bool actionDown(GameAction action);
inline bool actionUp(GameAction action);
