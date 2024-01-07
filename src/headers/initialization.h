#pragma once

#include "./engine_lib.h"
#include "./renderer.h"

#include "../deps/imgui/backends/imgui_impl_opengl3.h"
#include "../deps/imgui/backends/imgui_impl_sdl2.h"
#include "../deps/imgui/imgui.h"

// bumpAlloc to store logs and read shader sources
bool initSDLandGL(ProgramState *ps, GLContext *rs, RenderData *renderData,
                  BumpAllocator *transientStorage);

bool initImgui(ImguiState *imgui, ProgramState *pState);

bool initGL(GLContext *rs, BumpAllocator *transientStorage,
            RenderData *renderData);
void close(ProgramState *ps, GLContext *rs);

void printProgramLog(GLuint program, BumpAllocator *transientStorage);
void printShaderLog(GLuint shader, BumpAllocator *transientStorage);
