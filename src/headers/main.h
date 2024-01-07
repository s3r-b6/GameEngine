#pragma once

#include "./engine_lib.h"
#include "./game.h"
#include "./initialization.h"
#include "./renderer.h"

// Game shared object stub
typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void render();
void handleSDLevents(SDL_Event *event);
void updateGame(GameState *gameStateIn, RenderData *renderDataIn,
                Input *inputIn, ImguiState *gImgui, float dt);
void reloadGameLib(BumpAllocator *transientStorage);
