#pragma once

#include "engine_lib.h"

#include "initialization.h"

#include "renderer.h"

#include "game.h"

// Game shared object stub
typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void render();
void handleSDLevents(SDL_Event *event);
void updateGame(RenderData *renderDataIn);
void reloadGameLib(BumpAllocator *transientStorage);