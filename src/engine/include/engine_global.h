#pragma once

#include "./engine_memory.h"
#include "./types.h"

struct GlobalState {
    RenderData *renderData;
    GLContext *glContext;
    ProgramState *appState;
    GameState *gameState;
    Input *input;
    ALState *alState;
};

global BumpAllocator *permStorage;
global BumpAllocator *tempStorage;

global GlobalState *g;
