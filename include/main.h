#pragma once

#include "./game.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"

// Game shared object stub
typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void render();
void handleSDLevents(SDL_Event *event);
void updateGame(GlobalState *globalStateIn, float dt);
void reloadGameLib(BumpAllocator *transientStorage);
