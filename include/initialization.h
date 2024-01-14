#pragma once

#include "./types.h"
#include <SDL2/SDL.h>

// bumpAlloc to store logs and read shader sources
bool initSDLandGL(BumpAllocator *tempStorage, ProgramState *appState, GLContext *glContext,
                  RenderData *renderData);

bool initGL(BumpAllocator *tempStorage, GLContext *glContext, RenderData *renderData);

bool initImgui(ImguiState *imgui, ProgramState *appState);
void close(GLContext *glContext, ProgramState *appState);

void printProgramLog(uint program, BumpAllocator *transientStorage);
void printShaderLog(uint shader, BumpAllocator *transientStorage);
