#pragma once

#include <functional>
#include <vector>

#include "engine_lib.h"

enum GameAction {
    UNKNOWN,

    HELP,

    MOVE_U,
    MOVE_D,
    MOVE_L,
    MOVE_R,
    QUIT,

    TILE_1,
    TILE_2,
    TILE_3,

    LAYER_FRONT,
    LAYER_BACK,

    SAVE_WORLD,
    DELETE_WORLD,
    RELOAD_WORLD,

    GAME_ACTION_COUNT,
};

struct InputManager {
    std::vector<std::function<void(u32)>> inputActions;

    InputManager() { inputActions = std::vector<std::function<void(u32)>>(); }

    void registerFunction(GameAction action, char key, std::function<void(u32)> fn) {}
    void registerAction(GameAction action, char key, std::function<void(u32)> fn) {}

    void update() {}
};
