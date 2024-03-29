#pragma once

#include "./game.h"
#include "SDL2/SDL_events.h"

// Game shared object stub
typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void handleSDLevents(SDL_Event *event);
void updateGame(BumpAllocator *permStorage, BumpAllocator *tempStorage, GlobalState *globalStateIn,
                double dt);
void reloadGameLib(BumpAllocator *transientStorage);
