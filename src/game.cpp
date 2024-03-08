// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "SDL2/SDL_log.h"

#include "./game.h"

#include "./globals.h"

#include "./game_render.h"
#include "./renderer.h"

#include "./entities.h"
#include "./input.h"
#include "assets.h"

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
global TileSelection selection;

global u32 player_id;
global Entity *player;
global TransformComponent *transform;
global AnimatedSpriteRenderer *spriteRenderer;
global ColliderComponent *collider;

global float deltaTime;
global bool helpShown = false;

EXPORT_FN void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                          GlobalState *globalStateIn, float dt) {
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
        input = g->input;
        selection = gameState->selection;

        if (gameState->initialized) {
            player = gameState->entityManager->entities[player_id];
            transform = player->findComponent<TransformComponent>();
            spriteRenderer = player->findComponent<AnimatedSpriteRenderer>();
            collider = player->findComponent<ColliderComponent>();
        }
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

    if (helpShown) {
        // This is a placeholder...
        ui_drawTextFormatted(renderData, {20, 25}, 0.3, "%s: %c", "HELP", '/');
        ui_drawTextFormatted(renderData, {20, 50}, 0.3, "%s: %c", "MOVE_UP", 'w');
        ui_drawTextFormatted(renderData, {20, 75}, 0.3, "%s: %c", "MOVE_RIGHT", 'a');
        ui_drawTextFormatted(renderData, {20, 100}, 0.3, "%s: %c", "MOVE_DOWN", 's');
        ui_drawTextFormatted(renderData, {20, 125}, 0.3, "%s: %c", "MOVE_LEFT", 'd');
        ui_drawTextFormatted(renderData, {20, 150}, 0.3, "%s: %c", "TILE_1", '1');
        ui_drawTextFormatted(renderData, {20, 175}, 0.3, "%s: %c", "TILE_2", '2');
        ui_drawTextFormatted(renderData, {20, 200}, 0.3, "%s: %c", "TILE_3", '3');
        ui_drawTextFormatted(renderData, {20, 225}, 0.3, "%s: %c", "LAYER_FRONT", 'f');
        ui_drawTextFormatted(renderData, {20, 250}, 0.3, "%s: %c", "LAYER_BACK", 'b');
        ui_drawTextFormatted(renderData, {20, 275}, 0.3, "%s: %c", "SAVE_WORLD", '8');
        ui_drawTextFormatted(renderData, {20, 300}, 0.3, "%s: %c", "DELETE_WORLD", '9');
        ui_drawTextFormatted(renderData, {20, 325}, 0.3, "%s: %c", "RELOAD_WORLD", '0');
    }

    if (!pickerShown) {
        gameState->tileManager->renderBack(renderData);
        gameState->entityManager->render();
        gameState->tileManager->renderFront(renderData);

        ui_drawTextFormatted(renderData, {420, 15}, 0.2, "FPS:%d DT:%f", fps, dt);
        ui_drawTextFormatted(renderData, {420, 30}, 0.2, "playerPos:{%.2f, %.2f}", transform->pos.x,
                             transform->pos.y);
        ui_drawTextFormatted(renderData, {420, 45}, 0.2, "Tile:{ %d, %d } Layer: %d",
                             selection.selectedTile1.x, selection.selectedTile1.y,
                             selectedWorldLayer);

        if (selection.selectedTile2.atlasIdx) {
            ui_drawTileGroup(renderData, {selection.selectedTile1.x, selection.selectedTile1.y},
                             {selection.selectedTile2.x, selection.selectedTile2.y},
                             selection.selectedTile1.atlasIdx, input->mouseWorldPos * TILESIZE);
            ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, input->mouseWorldPos * TILESIZE);
        } else if (selection.selectedTile1.atlasIdx) {
            ui_drawTile(renderData, {selection.selectedTile1.x, selection.selectedTile1.y},
                        selection.selectedTile1.atlasIdx, input->mouseWorldPos * TILESIZE);
        }

        ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, input->mouseWorldPos * TILESIZE);
    }

    if (pickerShown) { drawTilePicker(WORLD_ATLAS, (int)1440, 40); }

    releaseActions(gameState, input);

    frame += 1;
}

void loadEntities() {
    player_id = gameState->entityManager->getUninitializedID();
    player = gameState->entityManager->entities[player_id];

    transform = new TransformComponent(glm::vec2(0, 0), glm::vec2(16, 32));
    spriteRenderer = new AnimatedSpriteRenderer(renderData, PlayerD_Walk, glm::vec2(16, 32),
                                                transform, 12, &deltaTime, 4, Player);
    collider = new ColliderComponent(transform, glm::vec2(16, 20));

    player->components.push_back(transform);
    player->components.push_back(spriteRenderer);
    player->components.push_back(collider);
}

inline void initializeGameState() {
    renderData->gameCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->gameCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};
    renderData->uiCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->uiCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};

    gameState->gameRegisterKey(HELP, '/');
    gameState->gameRegisterKey(MOVE_U, 'w');
    gameState->gameRegisterKey(MOVE_R, 'a');
    gameState->gameRegisterKey(MOVE_D, 's');
    gameState->gameRegisterKey(MOVE_L, 'd');
    gameState->gameRegisterKey(TILE_1, '1');
    gameState->gameRegisterKey(TILE_2, '2');
    gameState->gameRegisterKey(TILE_3, '3');
    gameState->gameRegisterKey(LAYER_FRONT, 'f');
    gameState->gameRegisterKey(LAYER_BACK, 'b');
    gameState->gameRegisterKey(SAVE_WORLD, '8');
    gameState->gameRegisterKey(DELETE_WORLD, '9');
    gameState->gameRegisterKey(RELOAD_WORLD, '0');

    gameState->entityManager->init(2048);
    loadEntities();

    selection.selectedTile1.atlasIdx = WORLD_ATLAS;
    selection.selectedTile1.x = 0;
    selection.selectedTile1.y = 0;

    if (gameState->tileManager->deserialize()) {
        log("Found previous world.data, loading the map", __FILE__, __LINE__);
    }

    gameState->initialized = true;
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

// I don't think I like this
bool checkTileCollisions() {
    for (int i = 0; i < gameState->tileManager->size; i++) {
        int x = i % WORLD_SIZE_x;
        int y = i / WORLD_SIZE_x;

        auto tile = gameState->tileManager->worldGridLayer1[i];
        if (!tile.atlasIdx) continue;

        if (collider->checkCollisions({x * TILESIZE, y * TILESIZE}, {16, 16})) return true;
    }

    return false;
}

void simulate() {
    if (!player) { crash("ERROR getting the player"); }
    if (!transform) { crash("ERROR getting the transform"); }
    if (!collider) { crash("ERROR getting the collider"); }

    if (pickerShown) return;

    auto oldPos = transform->pos;
    auto newPos = &transform->pos;

    bool moved = false;
    if (actionDown(gameState, input, MOVE_U)) {
        newPos->y -= playerSpeed;
        spriteRenderer->setAnimatedSprite(PlayerU_Walk);
        moved = true;
    } else if (actionDown(gameState, input, MOVE_D)) {
        newPos->y += playerSpeed;
        spriteRenderer->setAnimatedSprite(PlayerD_Walk);
        moved = true;
    }

    // TODO: This is the dumbest way to allow for movement when it is not possible in 1 direction.
    // this is horrible.
    // also: this might be too naive (maybe somehow the player can end stuck forever inside a
    // collision?)
    if (moved && checkTileCollisions()) {
        transform->setPos(oldPos);
        moved = false;
    }

    oldPos = transform->pos;

    if (actionDown(gameState, input, MOVE_R)) {
        newPos->x -= playerSpeed;
        spriteRenderer->setAnimatedSprite(PlayerR_Walk);
        moved = true;
    } else if (actionDown(gameState, input, MOVE_L)) {
        newPos->x += playerSpeed;
        spriteRenderer->setAnimatedSprite(PlayerL_Walk);
        moved = true;
    }

    if (moved && checkTileCollisions()) {
        transform->setPos(oldPos);
        moved = false;
    }

    spriteRenderer->setActive(moved);
    gameState->entityManager->update();
}

// TODO: This is terrible. Instead I should have some kind of "action mode". F.ex., in UI
// mode, check for all UI actions, and so on
void handleInput() {
    if (actionJustPressed(gameState, input, HELP)) { helpShown = !helpShown; }

    if (!pickerShown) {
        if (input->lMouseDown()) {
            if (selection.selectedTile2.atlasIdx) {
                gameState->tileManager->setTiles(input->mouseWorldPos, selection.selectedTile1,
                                                 selection.selectedTile2, selectedWorldLayer);
            } else if (selection.selectedTile1.atlasIdx) {
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
            log("Back layer", __FILE__, __LINE__);
            selectedWorldLayer = 0;
        } else if (actionJustPressed(gameState, input, LAYER_FRONT)) {
            log("Front layer", __FILE__, __LINE__);
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

                log("SELECTED TILES: 1{%d %d} 2{%d %d}", selection.selectedTile1.x,
                    selection.selectedTile1.y, selection.selectedTile2.x,
                    selection.selectedTile2.y);
            } else {
                selection.selectedTile1.atlasIdx = WORLD_ATLAS;
                selection.selectedTile2 = {0};
                log("SELECTED TILE: {%d %d}", selection.selectedTile1.x, selection.selectedTile1.y);
            }
        }

        if (actionDown(gameState, input, MOVE_U)) {
            renderData->uiCamera.pos.y -= 16;
        } else if (actionDown(gameState, input, MOVE_D)) {
            renderData->uiCamera.pos.y += 16;
        }
        if (!pickerShown && actionDown(gameState, input, MOVE_R)) {
            renderData->uiCamera.pos.x -= 16;
        } else if (!pickerShown && actionDown(gameState, input, MOVE_L)) {
            renderData->uiCamera.pos.x += 16;
        }
    } else {
        if (actionJustPressed(gameState, input, TILE_1)) { pickerShown = true; }
    }
}
