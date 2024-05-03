#pragma once

#include <cstdio>
#include <vector>

#include "SDL2/SDL_keyboard.h"

#include "./game.h"
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
EXPORT_FN void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                          RenderData *renderDataIn, ProgramState *appStateIn,
                          GameState *gameStateIn, GLContext *glContextIn, Input *inputIn,
                          double dt);
}

void renderWorld(int fps, double dt);
void initializeGameState();
