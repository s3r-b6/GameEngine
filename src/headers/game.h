#pragma once

#include "input.h"
#include "platform.h"
#include "renderer.h"

constexpr glm::ivec2 WORLD_SIZE = {320, 180};
constexpr int TILESIZE = 16;

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
