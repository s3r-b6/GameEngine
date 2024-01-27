#pragma once

#include "./mem.h"
#include "./types.h"

struct GlobalState {
    RenderData *renderData;
    GLContext *glContext;
    ProgramState *appState;
    GameState *gameState;
    Input *input;
};

// global BumpAllocator *permStorage = (BumpAllocator *)new BumpAllocator(MB(10));
// global BumpAllocator *tempStorage = (BumpAllocator *)new BumpAllocator(MB(10));

global BumpAllocator *permStorage;
global BumpAllocator *tempStorage;

global GlobalState *g;
