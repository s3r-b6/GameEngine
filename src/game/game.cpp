// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "./game.h"

#include "./engine_global.h"
#include "./game_global.h"

#include "./entities.h"
#include "./input.h"
#include "./renderer.h"
#include "./tiles.h"

#include "./game_input.h"

EXPORT_FN void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                          GlobalState *globalStateIn, double dt) {
    int fps = 1.f / dt;
    deltaTime = dt;

    // Since this is compiled as a separate dll, it holds its own static data
    if (g != globalStateIn) {
        permStorage = permStorageIn;
        tempStorage = tempStorageIn;
        g = globalStateIn;

        renderData = g->renderData;
        appState = g->appState;
        gameState = g->gameState;
        glContext = g->glContext;
        engine_input = g->input;
    }

    if (!gameState->initialized) initializeGameState();
    updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (updateTimer >= UPDATE_DELAY) {
        updateTimer -= UPDATE_DELAY;

        if (g->input->rMouseJustPressed()) { engine_log("%f R press", dt); }
        if (g->input->lMouseJustPressed()) { engine_log("%f L press", dt); }
        if (g->input->rMouseJustReleased()) { engine_log("%f R release", dt); }
        if (g->input->lMouseJustReleased()) { engine_log("%f L release", dt); }

        engine_input->mouseWorldPos =
            renderData->gameCamera.getMousePosInWorld(engine_input->mousePos, appState->screenSize);

        simulate();
    }

    renderWorld(fps, dt);

    frame += 1;
}

void renderWorld(int fps, double dt) {
    gameState->tileManager->renderBack(renderData);
    if (!gameState->tileManager->tilePickerShown) gameState->entityManager->render();
    gameState->tileManager->renderFront(renderData);

    if (!gameState->tileManager->tilePickerShown) {
        ui_drawTextFormatted(renderData, {420, 15}, 0.2, "FPS:%d DT:%f", fps, dt);

        if (gameState->tileManager->selection.selectedTile2.atlasIdx) {
            ui_drawTileGroup(renderData,
                             {gameState->tileManager->selection.selectedTile1.x,
                              gameState->tileManager->selection.selectedTile1.y},
                             {gameState->tileManager->selection.selectedTile2.x,
                              gameState->tileManager->selection.selectedTile2.y},
                             gameState->tileManager->selection.selectedTile1.atlasIdx,
                             g->input->mouseWorldPos * TILESIZE);
            ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, g->input->mouseWorldPos * TILESIZE);
        } else if (gameState->tileManager->selection.selectedTile1.atlasIdx) {
            ui_drawTile(renderData,
                        {gameState->tileManager->selection.selectedTile1.x,
                         gameState->tileManager->selection.selectedTile1.y},
                        gameState->tileManager->selection.selectedTile1.atlasIdx,
                        g->input->mouseWorldPos * TILESIZE);
        }
    }

    ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, engine_input->mouseWorldPos * TILESIZE);
}

void setupPlayer() {
    gameState->player_id = gameState->entityManager->getUninitializedID();
    auto player = gameState->entityManager->entities[gameState->player_id];
    auto transform = new (permStorage->alloc(sizeof(TransformComponent)))
        TransformComponent(glm::vec2(128, 128), glm::vec2(16, 32));
    player->components.push_back(transform);
    auto spriteRenderer = new (permStorage->alloc(sizeof(AnimatedSpriteRenderer)))
        AnimatedSpriteRenderer(gameState->player_id, renderData, PlayerD_Walk, {16, 32}, 12,
                               &deltaTime, 4, Player);
    player->components.push_back(spriteRenderer);

    auto collider = new (permStorage->alloc(sizeof(ColliderComponent)))
        ColliderComponent(gameState->player_id, {16, 20});
    player->components.push_back(collider);
}

inline void initializeGameState() {
    renderData->gameCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->gameCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};
    renderData->uiCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->uiCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};

    setupPlayer();

    gameState->tileManager->selection.selectedTile1.atlasIdx = WORLD_ATLAS;
    gameState->tileManager->selection.selectedTile1.x = 0;
    gameState->tileManager->selection.selectedTile1.y = 0;

    if (gameState->tileManager->deserialize()) {}

    gameState->initialized = true;
}

void simulate() {
    gameState->entityManager->update();
    inputFunctions(gameState->player_id);
}
