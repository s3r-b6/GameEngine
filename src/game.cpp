#include "game.h"

constexpr glm::ivec2 WORLD_SIZE = {320, 180};
constexpr int TILESIZE = 16;

EXPORT_FN void updateGame(GameState *gameStateIn, RenderData *renderDataIn,
                          Input *inputIn) {
    // Since this is compiled as a separate dll, it holds its own static data
    if (gRenderData != renderDataIn) {
        gRenderData = renderDataIn;
        gGameState = gameStateIn;
        gInput = inputIn;
    }

    if (!gGameState->initialized) {
        gGameState->playerPos = {0, 0};
        gRenderData->gameCamera = {
            .pos = {0, 0},
            .dimensions = {WORLD_SIZE.x, WORLD_SIZE.y},
        };

        gGameState->initialized = true;
    }

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            draw_sprite(gRenderData, Chest, {64.f * x, 64.f * y}, {16.f, 16.f});
            draw_sprite(gRenderData, Player, {32.f * x, 32.f * y},
                        {16.f, 16.f});
        }
    }
}
