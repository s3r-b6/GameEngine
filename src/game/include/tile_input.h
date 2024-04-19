#pragma once

#include "./entities.h"
#include "./game.h"
#include "./game_global.h"
#include "./input.h"
#include "./renderer.h"
#include "./tiles.h"

void handleTilePickerNavigation() {
    auto &selection = gameState->tileManager->selection;
    if (input->keyJustPressed('-')) {
        selection.selectedTile1 = {0, 0, gameState->tileManager->shownAtlas};
        selection.selectedTile2 = {0};

        gameState->tileManager->shownAtlas--;
        if (gameState->tileManager->shownAtlas < 2) { gameState->tileManager->shownAtlas = 2; }
        // engine_log("Tilemap selected: %u", gameState->tileManager->shownAtlas);
    } else if (input->keyJustPressed('=')) {
        gameState->tileManager->shownAtlas++;
        if (gameState->tileManager->shownAtlas > glContext->usedTextures) {
            gameState->tileManager->shownAtlas = glContext->usedTextures;
        }
        // engine_log("Tilemap selected: %u", gameState->tileManager->shownAtlas);
    }
}

void adjustSelectionBounds(glm::ivec2 mousePos) {
    auto &selection = gameState->tileManager->selection;
    u8 minX, minY, maxX, maxY;

    minX = std::min(selection.selectedTile1.x, (u8)(mousePos.x));
    maxX = std::max(selection.selectedTile1.x, (u8)(mousePos.x));
    minY = std::min(selection.selectedTile1.y, (u8)(mousePos.y));
    maxY = std::max(selection.selectedTile1.y, (u8)(mousePos.y));

    selection.selectedTile1 = {minX, minY};
    selection.selectedTile2 = {maxX, maxY};

    selection.selectedTile2.atlasIdx = gameState->tileManager->shownAtlas;
    selection.selectedTile1.atlasIdx = gameState->tileManager->shownAtlas;

    // SDL_Log("SELECTED TILES: 1{%d %d} 2{%d %d}", selection.selectedTile1.x,
    //         selection.selectedTile1.y, selection.selectedTile2.x, selection.selectedTile2.y);
}

void handleTileSelection() {
    auto &mousePos = input->mouseWorldPos;
    auto &selection = gameState->tileManager->selection;

    if (input->lMouseJustPressed()) {
        selection.selectedTile1 = {(u8)(mousePos.x), (u8)(mousePos.y),
                                   gameState->tileManager->shownAtlas};
        selection.selectedTile2 = {0};
    }
    if (input->lMouseJustReleased()) {
        if (selection.selectedTile1.x != (u8)(mousePos.x) ||
            selection.selectedTile1.y != (u8)(mousePos.y)) {
            adjustSelectionBounds(mousePos);
        } else {
            selection.selectedTile1.atlasIdx = gameState->tileManager->shownAtlas;
            selection.selectedTile2 = {0};
            SDL_Log("SELECTED TILE: {%d %d}", selection.selectedTile1.x, selection.selectedTile1.y);
        }
    }
}

void handleTileActions() {
    bool pickerShown = g->gameState->tileManager->tilePickerShown;

    if (g->input->keyJustPressed('1')) {
        g->gameState->tileManager->tilePickerShown = !pickerShown;
        pickerShown = !pickerShown;
        engine_log("engine_shown: %b", pickerShown);
    }

    if (!pickerShown) {
        if (input->keyJustPressed('[')) {
            engine_log("Saving world state");
            gameState->tileManager->serialize();
        } else if (input->keyJustPressed(']')) {
            engine_log("Reloading world");
            gameState->tileManager->deserialize();
        } else if (input->keyJustPressed('p')) {
            engine_log("Clearing tile manager");
            gameState->tileManager->clear();
        }

        auto &selection = gameState->tileManager->selection;
        if (input->lMouseDown()) {
            if (selection.selectedTile2.atlasIdx) {
                gameState->tileManager->setTiles(input->mouseWorldPos, selection.selectedTile1,
                                                 selection.selectedTile2, selection.selectedLayer);
            } else if (selection.selectedTile1.atlasIdx) {
                auto pos = input->mouseWorldPos;
                gameState->tileManager->setTile(pos.x, pos.y, selection.selectedTile1,
                                                selection.selectedLayer);
            }
        } else if (input->rMouseDown()) {
            gameState->tileManager->removeTile(input->mouseWorldPos.x, input->mouseWorldPos.y,
                                               gameState->tileManager->selection.selectedLayer);
        }

        return;
    }

    handleTilePickerNavigation();
    handleTileSelection();

    if (renderData->uiCamera.pos.y < 500 && input->keyIsDown('s')) {
        renderData->uiCamera.pos.y += TILESIZE;
    } else if (renderData->uiCamera.pos.y > 182 && input->keyIsDown('w')) {
        renderData->uiCamera.pos.y -= TILESIZE;
    }
}
