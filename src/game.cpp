#include "game.h"
#include "input.h"
#include "renderer.h"

constexpr double UPDATE_DELAY = 1. / 60.;

inline void initializeGameState() {
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

EXPORT_FN void updateGame(GameState *gameStateIn, RenderData *renderDataIn,
                          Input *inputIn, double dt) {
    // Since this is compiled as a separate dll, it holds its own static data
    if (gRenderData != renderDataIn) {
        gRenderData = renderDataIn;
        gGameState = gameStateIn;
        gInput = inputIn;
    }

    if (!gGameState->initialized) initializeGameState();

    gGameState->updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (gGameState->updateTimer >= UPDATE_DELAY) {
        gGameState->updateTimer -= UPDATE_DELAY;

        if (actionDown(MOVE_UP)) { gGameState->playerPos.y -= 2; }
        if (actionDown(MOVE_DOWN)) { gGameState->playerPos.y += 2; }
        if (actionDown(MOVE_RIGHT)) { gGameState->playerPos.x -= 2; }
        if (actionDown(MOVE_LEFT)) { gGameState->playerPos.x += 2; }
        gInput->mouseWorldPos = gInput->mousePos / (WORLD_SIZE / TILESIZE);
    }

    SDL_Log("FPS: %d, deltaTime: %f, updateDelay: %f", (int)(1 / dt), dt,
            UPDATE_DELAY);

    draw_sprite(gRenderData, Player, gGameState->playerPos, {16.f, 16.f});
}
