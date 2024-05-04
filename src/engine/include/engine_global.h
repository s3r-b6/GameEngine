#pragma once

#include "./types.h"

_global RenderData *renderData;
_global GLContext *glContext;
_global ProgramState *appState;
_global GameState *gameState;

_global TileManager *tileManager;
_global EntityManager *entityManager;

_global Input *input;
_global ALState *alState;

_global double deltaTime;
_global double updateTimer;
_global u64 frame;

_global BumpAllocator *permStorage;
_global BumpAllocator *tempStorage;
