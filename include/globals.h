#pragma once

#include "./types.h"

#include "./mem.h"

struct RenderData;
struct GLContext;
struct ProgramState;
struct GameState;
struct Input;

struct GlobalState {
    RenderData *renderData;
    GLContext *glContext;
    ProgramState *appState;
    GameState *gameState;
    Input *input;
};

global GlobalState *g;

global BumpAllocator *permStorage = new BumpAllocator(MB(10));
global BumpAllocator *tempStorage = new BumpAllocator(MB(10));

#define MAX_TEXTURES 8
#define UPDATE_DELAY 1. / 60.
#define MAX_TRANSFORMS 256
#define WORLD_SIZE_x 640
#define WORLD_SIZE_y 360
#define TILESIZE 16
