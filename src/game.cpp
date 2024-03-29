// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.
#include "./game.h"

#include "./entities.h"
#include "./renderer.h"

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

global double deltaTime;
global bool helpShown = false;

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
        input = g->input;
        selection = gameState->selection;
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

    if (helpShown)
        ui_drawTextFormatted(renderData, {20, 25}, 0.3,
                             "%s: %c\n%s: %c\n%s: %c\n%s: %c\n%s: %c\n%s: %c\n%s: %c\n%s: %c\n%s: "
                             "%c\n%s: %c\n%s: %c\n%s: %c\n%s: %c",
                             "HELP", '/', "MOVE_UP", 'w', "MOVE_RIGHT", 'a', "MOVE_DOWN", 's',
                             "MOVE_LEFT", 'd', "TILE_1", '1', "TILE_2", '2', "TILE_3", '3',
                             "LAYER_FRONT", 'f', "LAYER_BACK", 'b', "SAVE_WORLD", '8',
                             "DELETE_WORLD", '9', "RELOAD_WORLD", '0');

    if (!pickerShown) {
        gameState->tileManager->renderBack(renderData);
        gameState->entityManager->render();
        gameState->tileManager->renderFront(renderData);

        ui_drawTextFormatted(renderData, {420, 15}, 0.2, "FPS:%d DT:%f", fps, dt);
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

// TODO: I don't like this
bool checkTileCollisions(ColliderComponent *collider) {
    for (int i = 0; i < gameState->tileManager->size; i++) {
        int x = i % WORLD_SIZE_x;
        int y = i / WORLD_SIZE_x;

        auto tile = gameState->tileManager->worldGridLayer1[i];
        if (!tile.atlasIdx) continue;

        if (collider->checkCollisions({x * TILESIZE, y * TILESIZE}, {16, 16})) return true;
    }

    return false;
}

void setupPlayer() {
    player_id = gameState->entityManager->getUninitializedID();
    auto player = gameState->entityManager->entities[player_id];

    auto transform = new (permStorage->alloc(sizeof(TransformComponent)))
        TransformComponent(glm::vec2(0, 0), glm::vec2(16, 32));
    player->components.push_back(transform);

    auto spriteRenderer = new (permStorage->alloc(sizeof(AnimatedSpriteRenderer)))
        AnimatedSpriteRenderer(player_id, renderData, PlayerD_Walk, {16, 32}, 12, &deltaTime, 4,
                               Player);
    player->components.push_back(spriteRenderer);

    auto collider =
        new (permStorage->alloc(sizeof(ColliderComponent))) ColliderComponent(player_id, {16, 20});
    player->components.push_back(collider);

    auto inputController =
        new (permStorage->alloc(sizeof(InputController))) InputController(player_id);

    inputController->registerAction(HELP, '/', [](u32 player_id) {
        if (actionJustPressed(gameState, input, HELP)) { helpShown = !helpShown; }
    });

    auto move = [](u32 player_id) {
        auto player = gameState->entityManager->entities[player_id];
        auto transform = player->findComponent<TransformComponent>();
        auto spriteRenderer = player->findComponent<AnimatedSpriteRenderer>();
        auto collider = player->findComponent<ColliderComponent>();

        auto oldPos = transform->pos;
        auto newPos = &transform->pos;

        bool moved = false;
        if (actionDown(gameState, input, MOVE_U)) {
            newPos->y -= playerSpeed;
            spriteRenderer->animatedSprite = PlayerU_Walk;
            moved = true;
        } else if (actionDown(gameState, input, MOVE_D)) {
            newPos->y += playerSpeed;
            spriteRenderer->animatedSprite = PlayerD_Walk;
            moved = true;
        }

        // TODO: This is the dumbest way to allow for movement when it is not possible in 1
        // direction. this is horrible. also: this might be too naive (maybe somehow the player
        // can end stuck forever inside a collision?)
        if (moved && checkTileCollisions(collider)) {
            transform->pos = oldPos;
            moved = false;
        }

        oldPos = transform->pos;

        if (actionDown(gameState, input, MOVE_R)) {
            newPos->x -= playerSpeed;
            spriteRenderer->animatedSprite = PlayerR_Walk;
            moved = true;
        } else if (actionDown(gameState, input, MOVE_L)) {
            newPos->x += playerSpeed;
            spriteRenderer->animatedSprite = PlayerL_Walk;
            moved = true;
        }

        if (moved && checkTileCollisions(collider)) {
            transform->pos = oldPos;
            moved = false;
        }

        spriteRenderer->animating = moved;
    };

    GameAction movActions[] = {MOVE_U, MOVE_R, MOVE_D, MOVE_L};
    char movKeys[] = {'w', 'a', 's', 'd'};
    inputController->registerFunction(&movActions[0], &movKeys[0], 4, move);

    GameAction tilePickerActions[] = {LAYER_FRONT, LAYER_BACK, SAVE_WORLD, RELOAD_WORLD,
                                      DELETE_WORLD};
    char tilePickerKeys[] = {'f', 'b', '8', '9', '0'};
    inputController->registerFunction(tilePickerActions, tilePickerKeys, 5, [](u32) {
        if (actionJustPressed(gameState, input, SAVE_WORLD)) {
            gameState->tileManager->serialize();
        } else if (actionJustPressed(gameState, input, RELOAD_WORLD)) {
            gameState->tileManager->deserialize();
        } else if (actionJustPressed(gameState, input, DELETE_WORLD)) {
            gameState->tileManager->clear();
        }

        if (actionJustPressed(gameState, input, LAYER_BACK)) {
            log("Back layer");
            selectedWorldLayer = 0;
        } else if (actionJustPressed(gameState, input, LAYER_FRONT)) {
            log("Front layer");
            selectedWorldLayer = 1;
        }
    });

    player->components.push_back(inputController);
}

inline void initializeGameState() {
    renderData->gameCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->gameCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};
    renderData->uiCamera.pos = {WORLD_SIZE_x / 2., WORLD_SIZE_y / 2.};
    renderData->uiCamera.dimensions = {CAMERA_SIZE_x, CAMERA_SIZE_y};

    gameState->gameRegisterKey(TILE_1, '1');
    gameState->gameRegisterKey(TILE_2, '2');
    gameState->gameRegisterKey(TILE_3, '3');

    setupPlayer();

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

void simulate() {
    if (pickerShown) return;

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
            } else if (selection.selectedTile1.atlasIdx) {
                auto pos = input->mouseWorldPos;
                gameState->tileManager->setTile(pos.x, pos.y, selection.selectedTile1,
                                                selectedWorldLayer);
            }
        } else if (input->rMouseDown()) {
            auto pos = input->mouseWorldPos;
            gameState->tileManager->removeTile(pos.x, pos.y, selectedWorldLayer);
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
                    minX = selected1.x, maxX = mousePos.x;
                } else {
                    maxX = selected1.x, minX = mousePos.x;
                }

                if (selected1.y < mousePos.y) {
                    minY = selected1.y, maxY = mousePos.y;
                } else {
                    maxY = selected1.y, minY = mousePos.y;
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
