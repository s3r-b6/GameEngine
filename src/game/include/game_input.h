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

    std::vector<GameAction *> inputActions;
    InputMode current_mode = GAME_MODE;

    void registerFunction(BumpAllocator *alloc, char *id, InputMode valid_mode, char key,
                          std::function<void(u32)> fn) {
        GameAction *action = (GameAction *)alloc->alloc(sizeof(GameAction));
        action->ID = id;
        action->valid_mode = valid_mode;
        action->fn = fn;
        action->key = key;

        inputActions.push_back(action);
    }

    // TODO: This causes crashes on hot reload
    void update(u32 entity_id) {
        for (auto action : inputActions) {
            if (action->valid_mode == current_mode && action->key == '\0') {
                action->fn(entity_id);
            } else if (action->valid_mode == current_mode &&
                       g->input->keyJustPressed(action->key)) {
                action->fn(entity_id);
                engine_log("Action %s triggered", action->ID);
            }
        }
    }
};

void setup_keys();
