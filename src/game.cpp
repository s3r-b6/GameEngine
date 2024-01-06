#include "game.h"
#include "renderer.h"

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
        gGameState->playerPos = {0, 0}; // Top left is 0, 0
        gRenderData->gameCamera = {
            .pos = {160.f, 90.f},
            .dimensions = {(float)WORLD_SIZE.x, (float)WORLD_SIZE.y},
        };

        gGameState->initialized = true;
    }

    draw_sprite(gRenderData, Player, {160.f, -90.f}, {16.f, 16.f});

    draw_sprite(gRenderData, Player, {0.f, 0.f}, {16.f, 16.f});

    draw_sprite(gRenderData, Player, {80.f, 45.f}, {16.f, 16.f});
    draw_sprite(gRenderData, Player, {300.f, 90.f}, {16.f, 16.f});
}
