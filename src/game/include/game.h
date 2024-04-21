#pragma once

#include <cstdio>
#include <vector>

#include "SDL2/SDL_keyboard.h"

#include "./platform.h"

#define UPDATE_GAME_PARAMS                                                                         \
    BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn, RenderData *renderDataIn,          \
        ProgramState *appStateIn, GameState *gameStateIn, GLContext *glContextIn, Input *inputIn,  \
        double dt

struct EntityManager;
struct TileManager;

struct GameState {
    bool initialized;
    EntityManager *entityManager;
    TileManager *tileManager;

    u32 player_id;
};

extern "C" {
EXPORT_FN void updateGame(UPDATE_GAME_PARAMS);
}

void renderWorld(int fps, double dt);
void initializeGameState();
