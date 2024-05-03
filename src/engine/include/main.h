#pragma once

#include <SDL2/SDL_events.h>

#include "./game.h"
#include "./types.h"

typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void updateGame(BumpAllocator *permStorageIn, BumpAllocator *tempStorageIn,
                RenderData *renderDataIn, ProgramState *appStateIn, GameState *gameStateIn,
                GLContext *glContextIn, Input *inputIn, double dt);

void handleSDLevents(SDL_Event *event);
void reloadGameLib(BumpAllocator *transientStorage);
