#pragma once

#include "engine_lib.h"

struct ProgramState {
    bool running = true;
    int screen_width = 640;
    int screen_height = 640;
    SDL_Window *window = NULL;
    SDL_GLContext context;

    BumpAllocator *permanentStorage;
    BumpAllocator *transientStorage;
};

struct RenderState {
    GLuint programID = 0;
    GLint vertexPos2DLocation = -1;
    GLuint VBO = 0;
    GLuint IBO = 0;
};

bool initSDLandGL(ProgramState *ps, RenderState *rs);
bool initGL(RenderState *rs);
void close(ProgramState *ps, RenderState *rs);

void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);
