#pragma once

#include <GL/glew.h>
#include <GL/glu.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "memory.h"
#include "types.h"

// Struct that holds all the data relevant to program execution.
//  \bool running -> Marks if the program should continue to run the main loop
//  \int screen_width -> Current screen width
//  \int screen_height -> Current screen height
//  \SDL_Window *window -> Pointer to the SDL_Window being used
//  \SDL_GLContext context -> The context of SDL
//  \SDL_Renderer *sdl_renderer -> The SDL renderer. Used to get screen size
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
    int width;
    int height;
    SDL_Window *window;
    SDL_GLContext glContext;
};

global ProgramState *gAppState;
