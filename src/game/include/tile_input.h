#pragma once

// TODO: I don't like this
#include "./entities.h"
#include "./game.h"
#include "./game_global.h"
#include "./input.h"
#include "./tiles.h"

void handletileActions() {
    if (input->keyJustPressed('=')) {
        engine_log("Saving world state");
        gameState->tileManager->serialize();
    } else if (input->keyJustPressed('9')) {
        engine_log("Reloading world");
        gameState->tileManager->deserialize();
    } else if (input->keyJustPressed('-')) {
        engine_log("Clearing tile manager");
        gameState->tileManager->clear();
    }

    if (input->keyJustPressed('b')) {
        engine_log("Back layer");
        // selectedWorldLayer = 0;
    } else if (input->keyJustPressed('f')) {
        engine_log("Front layer");
        // selectedWorldLayer = 1;
    }
}

void toggleTilePicker() {
    g->gameState->tileManager->tilePickerShown = !g->gameState->tileManager->tilePickerShown;
}
