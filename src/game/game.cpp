// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "./engine_global.h"

#include "./game.h"

#include "./entities.h"
#include "./input.h"
#include "./renderer.h"
#include "./tiles.h"

#include "./game_input.h"
#include "types.h"

EXPORT_FN void updateGame(UPDATE_GAME_PARAMS) {
    int fps = 1.f / dt;
    deltaTime = dt;

    // Since this is compiled as a separate dll, it holds its own static data
    if (renderData != renderDataIn) {
        permStorage = permStorageIn, tempStorage = tempStorageIn;
        renderData = renderDataIn, appState = appStateIn;
        glContext = glContextIn, input = inputIn;

        gameState = gameStateIn;
        entityManager = gameState->entityManager, tileManager = gameState->tileManager;
    }

    if (!gameState->initialized) initializeGameState();
    updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (updateTimer >= UPDATE_DELAY) {
        updateTimer -= UPDATE_DELAY;

        input->mouseWorldPos =
            renderData->gameCamera.getMousePosInWorld(input->mousePos, appState->screenSize);
        input->mouseUiPos =
            renderData->uiCamera.getMousePosInWorld(input->mousePos, appState->screenSize);

        inputFunctions();

        entityManager->update();
    }

    renderWorld(fps, dt);

    frame += 1;
}

void drawTileSelection() {
    if (tileManager->selection.selectedTile2.atlasIdx) {
        ui_drawTileGroup(
            {tileManager->selection.selectedTile1.x, tileManager->selection.selectedTile1.y},
            {tileManager->selection.selectedTile2.x, tileManager->selection.selectedTile2.y},
            tileManager->selection.selectedTile1.atlasIdx, input->mouseWorldPos * TILESIZE);
    } else if (tileManager->selection.selectedTile1.atlasIdx) {
        ui_drawTile(
            {tileManager->selection.selectedTile1.x, tileManager->selection.selectedTile1.y},
            tileManager->selection.selectedTile1.atlasIdx, input->mouseWorldPos * TILESIZE);
    }
}

void renderWorld(int fps, double dt) {
    if (!tileManager->tilePickerShown) {
        ui_drawTextFormatted({420, 15}, 0.2, "FPS:%d DT:%f", fps, dt);
        drawTileSelection();
        entityManager->render();
    }
    tileManager->render();
}

void setupPlayer() {
    gameState->player_id = entityManager->getUninitializedID();
    auto player = entityManager->entities[gameState->player_id];
    auto transform = new (permStorage->alloc(sizeof(TransformComponent)))
        TransformComponent(glm::vec2(128, 128), glm::vec2(16, 32));
    player->components.push_back(transform);
    auto spriteRenderer = new (permStorage->alloc(sizeof(AnimatedSpriteRenderer)))
        AnimatedSpriteRenderer(gameState->player_id, renderData, PlayerD_Walk, 8, &deltaTime, 4,
                               Player);
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

    for (int i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) {
        tileManager->world[0].chunkTiles[i] = 112;
    }

    setupPlayer();

    tileManager->selection.selectedTile1.atlasIdx = WORLD_ATLAS;
    tileManager->selection.selectedTile1.x = 0;
    tileManager->selection.selectedTile1.y = 0;

    tileManager->shownAtlas = 2;
    tileManager->selection = {0};

    tileManager->selection.selectedTile1.atlasIdx = WORLD_ATLAS;
    tileManager->selection.selectedTile1.x = 0;
    tileManager->selection.selectedTile1.y = 0;

    // if (tileManager->deserialize()) {}

    gameState->initialized = true;
}
