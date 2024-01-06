#pragma once

#include "input.h"
#include "platform.h"
#include "renderer.h"

struct GameState {
    glm::ivec2 playerPos;
    bool initialized;
};

global GameState *gGameState;

// TODO: In win32 we need __declspec(dllexport)
extern "C" {
EXPORT_FN void updateGame(GameState *gameStateIn, RenderData *renderDataIn,
                          Input *inputIn);
}
