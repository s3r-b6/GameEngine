#pragma once

#include <cstdio>
#include <vector>

#include "SDL2/SDL_keyboard.h"

#include "./platform.h"
#include "./types.h"

#include "./game_render.h"

struct EntityManager;
struct TileManager;

struct GameState {
    bool initialized;
    EntityManager *entityManager;
    TileManager *tileManager;
};

extern "C" {
EXPORT_FN void updateGame(BumpAllocator *permStorage, BumpAllocator *tempStorage,
                          GlobalState *globalStateIn, double dt);
}

void initializeGameState();
void simulate();
void drawTilePicker(int textureAtlas, int maxTiles, int tilesPerRow);