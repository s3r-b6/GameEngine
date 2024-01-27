#pragma once

#include "./types.h"
#include <SDL2/SDL.h>

GlobalState *initialize(BumpAllocator *permStorage, BumpAllocator *tempStorage);
inline bool initSDLandGL(BumpAllocator *tempStorage, ProgramState *appState, GLContext *glContext,
                         RenderData *renderData);

inline bool initGL(BumpAllocator *tempStorage, GLContext *glContext, RenderData *renderData);

void close(GLContext *glContext, ProgramState *appState);
void printProgramLog(uint program, BumpAllocator *tempStorage);
void printShaderLog(uint shader, BumpAllocator *tempStorage);
