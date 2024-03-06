// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "SDL2/SDL_log.h"

#include "./game.h"

#include "./engine_lib.h"
#include "./globals.h"

#include "./game_render.h"
#include "./renderer.h"

#include "./entities.h"
#include "./input.h"

global double updateTimer;
global u64 frame;

global float playerSpeed = 1.5f;

global u8 selectedWorldLayer = 0;
global bool pickerShown = false;

global ProgramState *appState;
global GLContext *glContext;
global RenderData *renderData;
global GameState *gameState;
global Input *input;

global std::shared_ptr<Entity> player;
global std::shared_ptr<TransformComponent> transform;
global std::shared_ptr<SpriteRenderer> spriteRenderer;
global std::shared_ptr<ColliderComponent> collider;

// TODO: This is a placeholder
// I should Implement a way of asking an entity for a specific type of
// component and an entity manager for an specific entity (maybe ID?)
void loadEntities() {
    if (gameState->entityManager->entities.size() == 0) {
        player = std::make_shared<Entity>();
        transform = std::make_shared<TransformComponent>(glm::vec2(0, 0), glm::vec2(16, 32));
        spriteRenderer =
            std::make_shared<SpriteRenderer>(renderData, Player, glm::vec2(16, 32), transform);
        collider = std::make_shared<ColliderComponent>(transform);

        player->components.push_back(transform);
        player->components.push_back(spriteRenderer);
        player->components.push_back(collider);
        gameState->entityManager->entities.push_back(player);
    }
}

inline void initializeGameState() {
    renderData->gameCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->gameCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};
    renderData->uiCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->uiCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};

    gameState->gameRegisterKey(MOVE_UP, 'w');
    gameState->gameRegisterKey(MOVE_RIGHT, 'a');
    gameState->gameRegisterKey(MOVE_DOWN, 's');
    gameState->gameRegisterKey(MOVE_LEFT, 'd');
    gameState->gameRegisterKey(TILE_1, '1');
    gameState->gameRegisterKey(TILE_2, '2');
    gameState->gameRegisterKey(TILE_3, '3');
    gameState->gameRegisterKey(LAYER_FRONT, 'f');
    gameState->gameRegisterKey(LAYER_BACK, 'b');
    gameState->gameRegisterKey(SAVE_WORLD, '8');
    gameState->gameRegisterKey(DELETE_WORLD, '9');
    gameState->gameRegisterKey(RELOAD_WORLD, '0');

    loadEntities();

    selection.selectedTile1.atlasIdx = WORLD_ATLAS;
    selection.selectedTile1.x = 0;
    selection.selectedTile1.y = 0;

    if (gameState->tileManager->deserialize()) {
        SDL_Log("Found previous world.data, loading the map");
    }

    gameState->initialized = true;
}

EXPORT_FN void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                          GlobalState *globalStateIn, float dt) {
    int fps = 1.f / dt;

    // Since this is compiled as a separate dll, it holds its own static data
    if (g != globalStateIn) {
        permStorage = permStorageIn;
        tempStorage = tempStorageIn;
        g = globalStateIn;

        renderData = g->renderData;
        appState = g->appState;
        gameState = g->gameState;
        glContext = g->glContext;
        input = g->input;
    }

    if (!gameState->initialized) initializeGameState();
    updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (updateTimer >= UPDATE_DELAY) {
        updateTimer -= UPDATE_DELAY;

        input->mouseWorldPos =
            renderData->gameCamera.getMousePosInWorld(input->mousePos, appState->screenSize);
        input->mouseUIpos =
            renderData->uiCamera.getMousePosInWorld(input->mousePos, appState->screenSize);

        handleInput();
        simulate();
    }

    if (!pickerShown) {
        gameState->tileManager->renderBack(renderData);
        gameState->entityManager->render();
        gameState->tileManager->renderFront(renderData);

        ui_drawText(renderData, {20, 190}, 0.1, "TEST");
        ui_drawText(renderData, {50, 50}, 0.2, "TEST");

        ui_drawTextFormatted(renderData, {300, 250}, 0.3, "playerPos:{%.2f, %.2f}",
                             transform->pos.x, transform->pos.y);

        ui_drawTextFormatted(renderData, {300, 50}, 0.3, "FPS:%d DT:%f", fps, dt);
        ui_drawTextFormatted(renderData, {200, 85}, 0.3, "Tile:{ %d, %d } Layer: %d",
                             selection.selectedTile1.x, selection.selectedTile1.y,
                             selectedWorldLayer);
        if (selection.selectedTile2.atlasIdx) {
            ui_drawTileGroup(renderData, {selection.selectedTile1.x, selection.selectedTile1.y},
                             {selection.selectedTile2.x, selection.selectedTile2.y},
                             selection.selectedTile1.atlasIdx, input->mouseWorldPos * TILESIZE);
            ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, input->mouseWorldPos * TILESIZE);
        } else {
            ui_drawTile(renderData, {selection.selectedTile1.x, selection.selectedTile1.y},
                        selection.selectedTile1.atlasIdx, input->mouseWorldPos * TILESIZE);
            ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, input->mouseWorldPos * TILESIZE);
        }
    }

    if (pickerShown) { drawTilePicker(WORLD_ATLAS, (int)1440, 40); }

    releaseActions(gameState, input);

    frame += 1;
}

// TODO: This can be obviously improved by drawing a single quad the size of the tilemap instead of
// drawing n quads
void drawTilePicker(int textureAtlas, int maxTiles, int tilesPerRow) {
    for (int i = 0; i < maxTiles; i++) {
        int x = i % tilesPerRow, y = i / tilesPerRow;

        int worldPosX = i * TILESIZE % WORLD_SIZE_x;
        int worldPosY = i * TILESIZE / WORLD_SIZE_x * TILESIZE;
        ui_drawTile(renderData, {x, y}, textureAtlas, {worldPosX, worldPosY});
    }

    if (selection.selectedTile2.atlasIdx) {
        int xTiles = selection.selectedTile2.x - selection.selectedTile1.x;
        int yTiles = selection.selectedTile2.y - selection.selectedTile1.y;

        for (int x = 0; x <= xTiles; x++) {
            for (int y = 0; y <= yTiles; y++) {
                ui_drawTile(
                    renderData, {39, 35}, textureAtlas,
                    {(selection.selectedTile1.x + x) * 16, (selection.selectedTile1.y + y) * 16});
            }
        }
    } else {
        ui_drawTile(renderData, {39, 35}, textureAtlas,
                    {selection.selectedTile1.x * 16, selection.selectedTile1.y * 16});
    }

    ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, input->mouseUIpos * TILESIZE);
}

void simulate() {
    if (!player) { crash("ERROR getting the player"); }
    if (!transform) { crash("ERROR getting the transform"); }
    if (!collider) { crash("ERROR getting the collider"); }

    if (pickerShown) return;

    for (int i = 0; i < gameState->tileManager->size; i++) {
        int x = i % WORLD_SIZE_x;
        int y = i / WORLD_SIZE_x;

        auto tile = gameState->tileManager->worldGridLayer1[i];

        if (!tile.atlasIdx) { continue; }
        if (collider->check_collision({x * TILESIZE, y * TILESIZE}, {16, 16})) {
            SDL_Log("Collision at %d %d", x, y);
        }
    }

    auto pos = transform->pos;
    if (actionDown(gameState, input, MOVE_UP)) {
        pos.y -= playerSpeed;
    } else if (actionDown(gameState, input, MOVE_DOWN)) {
        pos.y += playerSpeed;
    }

    if (actionDown(gameState, input, MOVE_RIGHT)) {
        pos.x -= playerSpeed;
    } else if (actionDown(gameState, input, MOVE_LEFT)) {
        pos.x += playerSpeed;
    }
    transform->setPos({pos.x, pos.y});

    gameState->entityManager->update();
}

// TODO: This is terrible. Instead I should have some kind of "action mode". F.ex., in UI
// mode, check for all UI actions, and so on
void handleInput() {
    if (!pickerShown) {
        if (input->lMouseDown()) {
            if (selection.selectedTile2.atlasIdx) {
                gameState->tileManager->setTiles(input->mouseWorldPos, selection.selectedTile1,
                                                 selection.selectedTile2, selectedWorldLayer);
            } else {
                auto pos = input->mouseWorldPos;
                gameState->tileManager->setTile(pos.x, pos.y, selection.selectedTile1,
                                                selectedWorldLayer);
            }
        } else if (input->rMouseDown()) {
            auto pos = input->mouseWorldPos;
            gameState->tileManager->removeTile(pos.x, pos.y, selectedWorldLayer);
        }

        if (actionJustPressed(gameState, input, SAVE_WORLD)) {
            gameState->tileManager->serialize();
        } else if (actionJustPressed(gameState, input, RELOAD_WORLD)) {
            gameState->tileManager->deserialize();
        } else if (actionJustPressed(gameState, input, DELETE_WORLD)) {
            gameState->tileManager->clear();
        }

        if (actionJustPressed(gameState, input, LAYER_BACK)) {
            SDL_Log("Back layer");
            selectedWorldLayer = 0;
        } else if (actionJustPressed(gameState, input, LAYER_FRONT)) {
            SDL_Log("Front layer");
            selectedWorldLayer = 1;
        }
    }

    if (pickerShown) {
        if (actionJustPressed(gameState, input, TILE_2)) {
            renderData->uiCamera.pos = renderData->gameCamera.pos; // TODO: ???
            pickerShown = false;
        }

        if (input->lMouseJustPressed()) {
            auto mousePos = input->mouseUIpos;
            selection.selectedTile1 = {(u8)mousePos.x, (u8)mousePos.y};
            selection.selectedTile2 = {0};
        } else if (input->lMouseJustReleased()) {
            auto mousePos = input->mouseUIpos;

            auto selected1 = selection.selectedTile1;
            if (selected1.x != (u8)mousePos.x || selected1.y != (u8)mousePos.y) {
                u8 minX, minY, maxX, maxY;

                if (selected1.x < mousePos.x) {
                    minX = selected1.x;
                    maxX = mousePos.x;
                } else {
                    maxX = selected1.x;
                    minX = mousePos.x;
                }

                if (selected1.y < mousePos.y) {
                    minY = selected1.y;
                    maxY = mousePos.y;
                } else {
                    maxY = selected1.y;
                    minY = mousePos.y;
                }

                selection.selectedTile1 = {minX, minY};
                selection.selectedTile2 = {maxX, maxY};

                selection.selectedTile2.atlasIdx = WORLD_ATLAS;
                selection.selectedTile1.atlasIdx = WORLD_ATLAS;

                SDL_Log("SELECTED TILES: 1{%d %d} 2{%d %d}", selection.selectedTile1.x,
                        selection.selectedTile1.y, selection.selectedTile2.x,
                        selection.selectedTile2.y);
            } else {
                selection.selectedTile1.atlasIdx = WORLD_ATLAS;
                selection.selectedTile2 = {0};
                SDL_Log("SELECTED TILE: {%d %d}", selection.selectedTile1.x,
                        selection.selectedTile1.y);
            }
        }

        if (actionDown(gameState, input, MOVE_UP)) {
            renderData->uiCamera.pos.y -= 16;
        } else if (actionDown(gameState, input, MOVE_DOWN)) {
            renderData->uiCamera.pos.y += 16;
        }
        if (!pickerShown && actionDown(gameState, input, MOVE_RIGHT)) {
            renderData->uiCamera.pos.x -= 16;
        } else if (!pickerShown && actionDown(gameState, input, MOVE_LEFT)) {
            renderData->uiCamera.pos.x += 16;
        }
    } else {
        if (actionJustPressed(gameState, input, TILE_1)) { pickerShown = true; }
    }
}
