#pragma once

#include "./types.h"

void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                RenderData *renderDataIn, ProgramState *appStateIn, GameState *gameStateIn,
                GLContext *glContextIn, Input *inputIn, double dt);

typedef decltype(updateGame) update_game_type;
_global update_game_type *updateGame_ptr;

void handleSDLevents(SDL_Event *event);
void reloadGameLib(BumpAllocator *transientStorage);
