// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "SDL2/SDL_log.h"

#include "./game.h"

#include "./engine_lib.h"
#include "./entities.h"
#include "./globals.h"
#include "./input.h"
#include "./renderer.h"

// NOTE: g is the GlobalState object

global float speed = 1.5f;

// An empty vector results in a crash; so this has to be called after a hot-reload for now
void loadEntities() {
    if (g->gameState->entityManager->entities.size() == 0) {
        auto playerEntity = std::make_shared<Entity>();
        auto transformComponent = std::make_shared<TransformComponent>(glm::vec2(0, 0));
        auto spriteRenderer = std::make_shared<SpriteRenderer>(
            g->renderData, Player, glm::vec2(16, 32), transformComponent);

        playerEntity->components.push_back(transformComponent);
        playerEntity->components.push_back(spriteRenderer);
        g->gameState->entityManager->entities.push_back(playerEntity);
    }
}

inline void initializeGameState() {
    g->gameState->updateTimer = 0;
    g->renderData->gameCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};

    g->renderData->gameCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};

    gameRegisterKey(MOVE_UP, 'w');
    gameRegisterKey(MOVE_RIGHT, 'a');
    gameRegisterKey(MOVE_DOWN, 's');
    gameRegisterKey(MOVE_LEFT, 'd');

    gameRegisterKey(TILE_1, '1');
    gameRegisterKey(TILE_2, '2');
    gameRegisterKey(TILE_3, '3');

    loadEntities();

    selectedTile.atlasIdx = WORLD_ATLAS;
    selectedTile.x = 0;
    selectedTile.y = 0;

    u32 data = selectedTile.serialize();
    selectedTile.deserialize(data);

    g->gameState->initialized = true;
}

EXPORT_FN void updateGame(GlobalState *globalStateIn, float dt) {
    int fps = 1.f / dt;

    // Since this is compiled as a separate dll, it holds its own static data
    if (g != globalStateIn) { g = globalStateIn; }

    if (!g->gameState->initialized) initializeGameState();

    g->gameState->updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (g->gameState->updateTimer >= UPDATE_DELAY) {
        g->gameState->updateTimer -= UPDATE_DELAY;

        g->input->mouseWorldPos = g->renderData->gameCamera.getMousePosInWorld(
            g->input->mousePos, g->appState->screenSize);

        if (g->input->mouseInWindow && g->input->mLeftDown) {
            auto pos = g->input->mouseWorldPos;
            g->gameState->tileManager->setTile(pos.x, pos.y, selectedTile);
            // SDL_Log("Placing tile at %d %d", pos.x, pos.y);
        }

        if (g->input->mouseInWindow && g->input->mRightDown) {
            auto pos = g->input->mouseWorldPos;
            g->gameState->tileManager->removeTile(pos.x, pos.y);
            // SDL_Log("Removing tile at %d %d", pos.x, pos.y);
        }

        draw_tile(g->renderData, selectedTile.x, selectedTile.y, selectedTile.atlasIdx,
                  g->input->mouseWorldPos * TILESIZE);

        simulate();
    }

    g->gameState->entityManager->render();
    g->gameState->tileManager->render();

    draw_text(g->renderData, FONT_ATLAS, {550, 5}, 12, "FPS:%d DT:%f", fps, dt);
}

void simulate() {
    // TODO: This is a placeholder
    auto entity = g->gameState->entityManager->entities.at(0);
    auto component = entity->components.at(0);

    // TODO: Implement a way of asking an entity for a specific type of
    // component and an entity manager for an specific entity (maybe ID?)
    if (auto transformComponent = std::dynamic_pointer_cast<TransformComponent>(component)) {
        // SDL_Log("The component is a TransformComponent.");
        auto pos = transformComponent->pos;

        if (actionDown(MOVE_UP)) { pos.y -= speed; }
        if (actionDown(MOVE_DOWN)) { pos.y += speed; }
        if (actionDown(MOVE_RIGHT)) { pos.x -= speed; }
        if (actionDown(MOVE_LEFT)) { pos.x += speed; }

        if (actionDown(TILE_1)) {
            selectedTile.x = 0;
            selectedTile.y = 0;
        }
        if (actionDown(TILE_2)) {
            selectedTile.x = 16;
            selectedTile.y = 0;
        }
        if (actionDown(TILE_3)) {
            selectedTile.x = 32;
            selectedTile.y = 0;
        }

        transformComponent->setPos(pos.x, pos.y);
    } else {
        SDL_Log("The component is not a TransformComponent.");
    }

    g->gameState->entityManager->update();
}
