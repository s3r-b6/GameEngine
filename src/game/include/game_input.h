#pragma once

#include <functional>
#include <vector>

#include "engine_global.h"
#include "engine_lib.h"
#include "input.h"

struct InputManager {
    enum InputMode {
        INVALID_MODE,
        ANY_MODE,
        GAME_MODE,
        UI_MODE,
        INPUT_MODE_COUNT,
    };

    struct GameAction {
        char *ID;
        InputMode valid_mode;
        std::function<void(u32)> fn;

        char key;
    };

    std::vector<GameAction> inputActions;
    InputMode current_mode;
    Input *engine_input;

    InputManager(Input *inputIn) {
        inputActions = std::vector<GameAction>();
        current_mode = GAME_MODE;
        engine_input = inputIn;
    }

    void registerFunction(char *id, InputMode valid_mode, char key, std::function<void(u32)> fn) {
        inputActions.push_back({id, valid_mode, fn, key});
    }

    void update(u32 entity_id) {
        for (auto &action : inputActions) {
            if (action.valid_mode == current_mode && action.key == '\0') {
                action.fn(entity_id);
            } else if (action.valid_mode == current_mode && engine_input->keyIsDown(action.key)) {
                action.fn(entity_id);
                engine_log("Action %s triggered", action.ID);
            };
        }
    }
};

void setup_keys();