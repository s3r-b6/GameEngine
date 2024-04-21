#pragma once

#include "./game.h"
#include <SDL2/SDL_events.h>

// Game shared object stub
typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void handleSDLevents(SDL_Event *event);
void reloadGameLib(BumpAllocator *transientStorage);

// This is the game function call
void updateGame(UPDATE_GAME_PARAMS);
