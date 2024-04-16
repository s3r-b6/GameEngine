#pragma once

#include "engine_lib.h"
#include "game_input.h"

global double deltaTime;
global double updateTimer;
global u64 frame;
global ProgramState *appState;
global GLContext *glContext;
global RenderData *renderData;
global GameState *gameState;
global InputManager *inputManager;
global Input *engine_input;
global u32 player_id;
