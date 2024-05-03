// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include <cstdlib>
#include <ctime>

#include "./engine_global.h"
#include "types.h"

#include "./game.h"

#include "./entities.h"
#include "./input.h"
#include "./renderer.h"
#include "./tiles.h"
#include "./rooms.h"

#include "./game_input.h"

EXPORT_FN void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                          RenderData *renderDataIn, ProgramState *appStateIn,
                          GameState *gameStateIn, GLContext *glContextIn, Input *inputIn,
                          double dt) {
    int fps = 1.f / dt;
    deltaTime = dt;

    // Since this is compiled as a separate dll, it holds its own static data
    if (renderData != renderDataIn) {
        engine_log("Assigning static data ptrs...");
        permStorage = permStorageIn, tempStorage = tempStorageIn;
        renderData = renderDataIn, appState = appStateIn;
        glContext = glContextIn, input = inputIn;
        gameState = gameStateIn;
        entityManager = gameState->entityManager, tileManager = gameState->tileManager;
    }

    if (!gameState->initialized) {
        engine_log("Initializing the world state...");
        initializeGameState();
    }
    updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (updateTimer >= UPDATE_DELAY) {
        updateTimer -= UPDATE_DELAY;

        input->mouseWorldPos =
            renderData->gameCamera.getMousePosInWorld(input->mousePos, appState->screenSize);
        input->mouseUiPos =
            renderData->uiCamera.getMousePosInWorld(input->mousePos, appState->screenSize);

        entityManager->update();
        inputFunctions();
    }

    renderWorld(fps, dt);

    frame += 1;
}

void renderWorld(int fps, double dt) {
    ui_drawTextFormatted({420, 15}, 0.2, "FPS:%d DT:%f", fps, dt);
    entityManager->render();
    tileManager->render();
}

void setupPlayer() {
    gameState->player_id = entityManager->getUninitializedID();
    auto player = entityManager->entities[gameState->player_id];
    auto transform = new (permStorage->alloc(sizeof(TransformComponent))) TransformComponent(
        glm::vec2(CHUNK_SIZE_x / 2 - 8, CHUNK_SIZE_y / 2 - 16), glm::vec2(16, 32));
    player->components.push_back(transform);
    auto spriteRenderer = new (permStorage->alloc(sizeof(AnimatedSpriteRenderer)))
        AnimatedSpriteRenderer(gameState->player_id, renderData, PlayerD_Walk, 8, &deltaTime, 4,
                               PlayerD);
    player->components.push_back(spriteRenderer);

    auto collider = new (permStorage->alloc(sizeof(ColliderComponent)))
        ColliderComponent(gameState->player_id, {16, 20});
    player->components.push_back(collider);
}

inline void initializeGameState() {
    renderData->gameCamera.pos = {TILES_CHUNK_x * 16. / 2., TILES_CHUNK_y * 16 / 2.};
    renderData->gameCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};
    renderData->uiCamera.pos = {TILES_CHUNK_x * 16. / 2., TILES_CHUNK_y * 16 / 2.};
    renderData->uiCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};

    engine_log("Setting up the player...");
    setupPlayer();

    tileManager->world[0].x = 0;
    tileManager->world[0].y = 0;
    tileManager->world[1].x = 1;
    tileManager->world[1].y = 0;
    tileManager->world[2].x = 2;
    tileManager->world[2].y = 0;
    tileManager->world[3].x = 1;
    tileManager->world[3].y = -1;
    engine_log("Loading room1 into the allocator");
    u16 *room1Mem = loadRoom(permStorage);
    tileManager->world[0].chunkTiles = room1Mem;
    tileManager->world[1].chunkTiles = room1Mem;
    tileManager->world[2].chunkTiles = room1Mem;
    tileManager->world[3].chunkTiles = room1Mem;

    engine_log("Initialized world state");
    gameState->initialized = true;
}
