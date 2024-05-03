#pragma once

#include "./types.h"

global RenderData *renderData;
global GLContext *glContext;
global ProgramState *appState;
global GameState *gameState;

global TileManager *tileManager;
global EntityManager *entityManager;

global Input *input;
global ALState *alState;

global double deltaTime;
global double updateTimer;
global u64 frame;

global BumpAllocator *permStorage;
global BumpAllocator *tempStorage;
