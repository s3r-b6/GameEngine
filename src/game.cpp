// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "SDL2/SDL_log.h"

#include "./game.h"

#include "./engine_lib.h"
#include "./globals.h"

#include "./entities.h"
#include "./input.h"
#include "./renderer.h"

// NOTE: g is the GlobalState object

global double updateTimer;
global u64 frame;

global float playerSpeed = 1.5f;

global u8 selectedWorldLayer = 0;
global bool pickerShown = false;

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
    g->renderData->gameCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    g->renderData->gameCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};
    g->renderData->uiCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    g->renderData->uiCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};

    g->gameState->gameRegisterKey(MOVE_UP, 'w');
    g->gameState->gameRegisterKey(MOVE_RIGHT, 'a');
    g->gameState->gameRegisterKey(MOVE_DOWN, 's');
    g->gameState->gameRegisterKey(MOVE_LEFT, 'd');
    g->gameState->gameRegisterKey(TILE_1, '1');
    g->gameState->gameRegisterKey(TILE_2, '2');
    g->gameState->gameRegisterKey(TILE_3, '3');
    g->gameState->gameRegisterKey(LAYER_FRONT, 'f');
    g->gameState->gameRegisterKey(LAYER_BACK, 'b');
    g->gameState->gameRegisterKey(SAVE_WORLD, '8');
    g->gameState->gameRegisterKey(DELETE_WORLD, '9');
    g->gameState->gameRegisterKey(RELOAD_WORLD, '0');

    loadEntities();

    selection.selectedTile1.atlasIdx = WORLD_ATLAS;
    selection.selectedTile1.x = 0;
    selection.selectedTile1.y = 0;

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
    updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (updateTimer >= UPDATE_DELAY) {
        updateTimer -= UPDATE_DELAY;

        g->input->mouseWorldPos = g->renderData->gameCamera.getMousePosInWorld(
            g->input->mousePos, g->appState->screenSize);
        g->input->mouseUIpos =
            g->renderData->uiCamera.getMousePosInWorld(g->input->mousePos, g->appState->screenSize);

        handleInput();
        simulate();
    }

    if (!pickerShown) {
        g->gameState->tileManager->renderBack(g->renderData);
        g->gameState->entityManager->render();
        g->gameState->tileManager->renderFront(g->renderData);

        ui_drawText(g->renderData, {20, 190}, 0.1, "TEST");
        ui_drawText(g->renderData, {50, 50}, 0.2, "TEST");
        ui_drawTextFormatted(g->renderData, {300, 50}, 0.3, "FPS:%d DT:%f", fps, dt);
        ui_drawTextFormatted(g->renderData, {200, 85}, 0.3, "Tile:{ %d, %d } Layer: %d",
                             selection.selectedTile1.x, selection.selectedTile1.y,
                             selectedWorldLayer);
        if (selection.selectedTile2.atlasIdx) {
            ui_drawTileGroup(g->renderData, {selection.selectedTile1.x, selection.selectedTile1.y},
                             {selection.selectedTile2.x, selection.selectedTile2.y},
                             selection.selectedTile1.atlasIdx, g->input->mouseWorldPos * TILESIZE);
            ui_drawTile(g->renderData, {39, 35}, WORLD_ATLAS, g->input->mouseWorldPos * TILESIZE);
        } else {
            ui_drawTile(g->renderData, {selection.selectedTile1.x, selection.selectedTile1.y},
                        selection.selectedTile1.atlasIdx, g->input->mouseWorldPos * TILESIZE);
            ui_drawTile(g->renderData, {39, 35}, WORLD_ATLAS, g->input->mouseWorldPos * TILESIZE);
        }
    }

    if (pickerShown) { drawTilePicker(WORLD_ATLAS, (int)1440, 40); }

    releaseActions(g->gameState, g->input);

    frame += 1;
}

void drawTilePicker(int textureAtlas, int maxTiles, int tilesPerRow) {
    for (int i = 0; i < maxTiles; i++) {
        int x = i % tilesPerRow, y = i / tilesPerRow;

        int worldPosX = i * TILESIZE % WORLD_SIZE_x;
        int worldPosY = i * TILESIZE / WORLD_SIZE_x * TILESIZE;
        ui_drawTile(g->renderData, {x, y}, textureAtlas, {worldPosX, worldPosY});
    }

    ui_drawTile(g->renderData, {39, 35}, WORLD_ATLAS, g->input->mouseUIpos * TILESIZE);
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

    if (!pickerShown) {
        if (transformComponent) {
            auto pos = transformComponent->pos;

            if (actionDown(g->gameState, g->input, MOVE_UP)) {
                pos.y -= playerSpeed;
            } else if (actionDown(g->gameState, g->input, MOVE_DOWN)) {
                pos.y += playerSpeed;
            }

            if (actionDown(g->gameState, g->input, MOVE_RIGHT)) {
                pos.x -= playerSpeed;
            } else if (actionDown(g->gameState, g->input, MOVE_LEFT)) {
                pos.x += playerSpeed;
            }

            transformComponent->setPos(pos.x, pos.y);
        } else {
            SDL_Log("The component is not a TransformComponent.");
        }
    }
    g->gameState->entityManager->update();
}

void handleInput() {
    if (!pickerShown) {
        if (g->input->lMouseDown()) {
            if (selection.selectedTile2.atlasIdx) {
                g->gameState->tileManager->setTiles(g->input->mouseWorldPos,
                                                    selection.selectedTile1,
                                                    selection.selectedTile2, selectedWorldLayer);
            } else {
                auto pos = g->input->mouseWorldPos;
                g->gameState->tileManager->setTile(pos.x, pos.y, selection.selectedTile1,
                                                   selectedWorldLayer);
            }
        } else if (g->input->rMouseDown()) {
            auto pos = g->input->mouseWorldPos;
            g->gameState->tileManager->removeTile(pos.x, pos.y, selectedWorldLayer);
        }

        if (actionJustPressed(g->gameState, g->input, SAVE_WORLD)) {
            g->gameState->tileManager->serialize();
        } else if (actionJustPressed(g->gameState, g->input, RELOAD_WORLD)) {
            g->gameState->tileManager->deserialize();
        } else if (actionJustPressed(g->gameState, g->input, DELETE_WORLD)) {
            g->gameState->tileManager->clear();
        }
    }

    if (actionJustPressed(g->gameState, g->input, LAYER_BACK)) {
        SDL_Log("Back layer");
        selectedWorldLayer = 0;
    } else if (actionJustPressed(g->gameState, g->input, LAYER_FRONT)) {
        SDL_Log("Front layer");
        selectedWorldLayer = 1;
    }

    if (pickerShown) {
        if (actionJustPressed(g->gameState, g->input, TILE_2)) {
            g->renderData->uiCamera.pos = g->renderData->gameCamera.pos; // TODO: ???
            pickerShown = false;
        }

        if (g->input->lMouseJustPressed()) {
            auto mousePos = g->input->mouseUIpos;
            selection.selectedTile1 = {(u8)mousePos.x, (u8)mousePos.y};
            selection.selectedTile1.atlasIdx = WORLD_ATLAS;
            SDL_Log("SELECTED TILE1: %d %d", selection.selectedTile1.x, selection.selectedTile1.y);
        } else if (g->input->lMouseJustReleased()) {
            auto mousePos = g->input->mouseUIpos;

            auto selected1 = selection.selectedTile1;

            if (selected1.x != (u8)mousePos.x || selected1.y != (u8)mousePos.y) {
                selection.selectedTile2.x = mousePos.x;
                selection.selectedTile2.y = mousePos.y;
                selection.selectedTile2.atlasIdx = WORLD_ATLAS;
            } else {
                selection.selectedTile2 = {0};
            }

            SDL_Log("SELECTED TILE2: %d %d", selection.selectedTile2.x, selection.selectedTile2.y);
        }

        if (actionDown(g->gameState, g->input, MOVE_UP)) {
            g->renderData->uiCamera.pos.y -= 16;
        } else if (actionDown(g->gameState, g->input, MOVE_DOWN)) {
            g->renderData->uiCamera.pos.y += 16;
        }
        if (actionDown(g->gameState, g->input, MOVE_RIGHT)) {
            g->renderData->uiCamera.pos.x -= 16;
        } else if (actionDown(g->gameState, g->input, MOVE_LEFT)) {
            g->renderData->uiCamera.pos.x += 16;
        }
    } else {
        if (actionJustPressed(g->gameState, g->input, TILE_1)) { pickerShown = true; }
    }
}
