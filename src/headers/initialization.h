#pragma once

#include "engine_lib.h"
#include "renderer.h"

// bumpAlloc to store logs and read shader sources
bool initSDLandGL(ProgramState *ps, GLContext *rs, RenderData *renderData,
                  BumpAllocator *transientStorage);
bool initGL(GLContext *rs, BumpAllocator *transientStorage,
            RenderData *renderData);
void close(ProgramState *ps, GLContext *rs);

void printProgramLog(GLuint program, BumpAllocator *transientStorage);
void printShaderLog(GLuint shader, BumpAllocator *transientStorage);
