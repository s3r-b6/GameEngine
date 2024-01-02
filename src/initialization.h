#pragma once

#include "engine_lib.h"

// Struct that holds all the data relevant to program execution.
//  \bool running -> Marks if the program should continue to run the main loop
//  \int screen_width -> Current screen width
//  \int screen_height -> Current screen height
//  \SDL_Window *window -> Pointer to the SDL_Window being used
//  \SDL_GLContext context -> The context of SDL
//  \BumpAllocator *permanentStorage; -> A *BumpAllocator that should *not* get
//                                      freed. For example, this should hold the
//                                      GameState, and so on.
//
//  \BumpAllocator *transientStorage; -> A *BumpAllocator that is expected to
//                                      get freed, since the data it holds is
//                                      temporal. For example, reading a shader
//                                      source file: that data is only useful
//                                      until it is compiled
struct ProgramState {
    bool running = true;
    int screen_width;
    int screen_height;
    SDL_Window *window;
    SDL_GLContext context;

    BumpAllocator *permanentStorage;
    BumpAllocator *transientStorage;
};

// Struct that holds all data relevant to the renderer
struct RenderState {
    GLuint programID = 0;
    GLint vertexPos2DLocation = -1;
    GLuint VBO = 0;
    GLuint IBO = 0;
};

// bumpAlloc to store logs and read shader sources
bool initSDLandGL(ProgramState *ps, RenderState *rs,
                  BumpAllocator *transientStorage);
bool initGL(RenderState *rs, BumpAllocator *transientStorage);
void close(ProgramState *ps, RenderState *rs);

void printProgramLog(GLuint program, BumpAllocator *transientStorage);
void printShaderLog(GLuint shader, BumpAllocator *transientStorage);
