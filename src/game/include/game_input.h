#pragma once

#include <functional>
#include <vector>

#include "engine_lib.h"
#include "game.h"

enum InputMode {
    INVALID_MODE,
    GAME_MODE,
    UI_MODE,
    INPUT_MODE_COUNT,
}

struct GameAction {
    InputMode valid_mode;
    std::function<void(u32)> fn;
    char key;

    bool triggered(u32 player_id) { 
        fn(player_id); 
        }
};

struct InputManager {
    std::vector<std::function<void(u32)>> inputActions;

    InputManager() { inputActions = std::vector<std::function<void(u32)>>(); }

    void registerFunction(InputMode valid_mode, char key, std::function<void(u32)> fn) {}

    void update() {}
};
