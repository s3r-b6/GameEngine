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
global Input *engine_input;
