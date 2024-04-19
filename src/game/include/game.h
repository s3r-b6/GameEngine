#pragma once

#include <cstdio>
#include <vector>

#include "SDL2/SDL_keyboard.h"

#include "./platform.h"

struct EntityManager;
struct TileManager;

struct GameState {
    bool initialized;
    EntityManager *entityManager;
    TileManager *tileManager;

    u32 player_id;
};

extern "C" {
EXPORT_FN void updateGame(BumpAllocator *permStorage, BumpAllocator *tempStorage,
                          GlobalState *globalStateIn, double dt);
void renderWorld(int fps, double dt);
}

void initializeGameState();
void simulate();
// void drawTilePicker(int textureAtlas, int maxTiles, int tilesPerRow);
