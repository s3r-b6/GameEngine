#pragma once

#include "./game.h"
#include "SDL2/SDL_events.h"

// Game shared object stub
typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void render();
void handleSDLevents(SDL_Event *event);
void updateGame(BumpAllocator *permStorage, BumpAllocator *tempStorage, GlobalState *globalStateIn,
                float dt);
void reloadGameLib(BumpAllocator *transientStorage);
