// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "SDL2/SDL_log.h"

#include "./game.h"

#include "./engine_lib.h"
#include "./entities.h"
#include "./globals.h"
#include "./input.h"
#include "./renderer.h"

#include "./platform.h"
#include "types.h"

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

    gameRegisterKey(g->gameState, g->input, MOVE_UP, 'w');
    gameRegisterKey(g->gameState, g->input, MOVE_RIGHT, 'a');
    gameRegisterKey(g->gameState, g->input, MOVE_DOWN, 's');
    gameRegisterKey(g->gameState, g->input, MOVE_LEFT, 'd');

    gameRegisterKey(g->gameState, g->input, TILE_1, '1');
    gameRegisterKey(g->gameState, g->input, TILE_2, '2');
    gameRegisterKey(g->gameState, g->input, TILE_3, '3');

    gameRegisterKey(g->gameState, g->input, SAVE_WORLD, '8');
    gameRegisterKey(g->gameState, g->input, DELETE_WORLD, '9');
    gameRegisterKey(g->gameState, g->input, RELOAD_WORLD, '0');

    loadEntities();

    selectedTile.atlasIdx = WORLD_ATLAS;
    selectedTile.x = 0;
    selectedTile.y = 0;

    if (g->gameState->tileManager->deserialize()) {
        SDL_Log("Found previous world.data, loading the map");
    }

    g->gameState->initialized = true;
}

EXPORT_FN void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                          GlobalState *globalStateIn, float dt) {
    int fps = 1.f / dt;

    // Since this is compiled as a separate dll, it holds its own static data
    if (g != globalStateIn) {
        permStorage = permStorageIn;
        tempStorage = tempStorageIn;
        g = globalStateIn;
    }

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
    g->gameState->tileManager->render(g->renderData);

    local_persist bool just_loaded = false;
    // TODO: This should use the tempStorage and write to a file
    if (actionJustPressed(g->gameState, g->input, SAVE_WORLD)) {
        just_loaded = false;
        g->gameState->tileManager->serialize();
    }

    if (!just_loaded && actionJustPressed(g->gameState, g->input, RELOAD_WORLD)) {
        just_loaded = true;
        g->gameState->tileManager->deserialize();
    }

    if (actionJustPressed(g->gameState, g->input, DELETE_WORLD)) {
        just_loaded = false;
        g->gameState->tileManager->clear();
    }

    draw_text(g->renderData, FONT_ATLAS, {550, 5}, 12, "FPS:%d DT:%f", fps, dt);
}

void simulate() {
    local_persist std::shared_ptr<Entity> entity;
    local_persist std::shared_ptr<EntityComponentBase> component;
    local_persist std::shared_ptr<TransformComponent> transformComponent;

    // TODO: This is a placeholder
    // I should Implement a way of asking an entity for a specific type of
    // component and an entity manager for an specific entity (maybe ID?)
    if (!entity) {
        entity = g->gameState->entityManager->entities.at(0);
        component = entity->components.at(0);
        transformComponent = std::dynamic_pointer_cast<TransformComponent>(component);
    }

    if (transformComponent) {
        // SDL_Log("The component is a TransformComponent.");
        auto pos = transformComponent->pos;

        if (actionDown(g->gameState, g->input, MOVE_UP)) { pos.y -= speed; }
        if (actionDown(g->gameState, g->input, MOVE_DOWN)) { pos.y += speed; }
        if (actionDown(g->gameState, g->input, MOVE_RIGHT)) { pos.x -= speed; }
        if (actionDown(g->gameState, g->input, MOVE_LEFT)) { pos.x += speed; }

        if (actionDown(g->gameState, g->input, TILE_1)) { selectedTile.x = 0, selectedTile.y = 0; }
        if (actionDown(g->gameState, g->input, TILE_2)) { selectedTile.x = 16, selectedTile.y = 0; }
        if (actionDown(g->gameState, g->input, TILE_3)) { selectedTile.x = 32, selectedTile.y = 0; }

        transformComponent->setPos(pos.x, pos.y);
    } else {
        SDL_Log("The component is not a TransformComponent.");
    }

    g->gameState->entityManager->update();
}
