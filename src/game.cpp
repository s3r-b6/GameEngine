#include "game.h"
#include "input.h"
#include "renderer.h"

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

        // Last keycode pressed is stored in gInput->lastPressed. To allow for
        // rebinds the idea right now is that we would check which GameAction is
        // being requested to remap, and, move it to the new (last pressed)
        // keycode
        gameRegisterKey(MOVE_UP, 'w');
        gameRegisterKey(MOVE_RIGHT, 'a');
        gameRegisterKey(MOVE_DOWN, 's');
        gameRegisterKey(MOVE_LEFT, 'd');

        gGameState->initialized = true;
    }

    draw_sprite(gRenderData, Player, gGameState->playerPos, {16.f, 16.f});

    if (actionDown(MOVE_UP)) { gGameState->playerPos.y -= 1; }
    if (actionDown(MOVE_DOWN)) { gGameState->playerPos.y += 1; }
    if (actionDown(MOVE_RIGHT)) { gGameState->playerPos.x -= 1; }
    if (actionDown(MOVE_LEFT)) { gGameState->playerPos.x += 1; }
}
