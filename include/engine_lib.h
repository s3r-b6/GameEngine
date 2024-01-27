#pragma once

#include "./types.h"

#include "SDL2/SDL_log.h"
#include "SDL2/SDL_video.h"

// Struct that holds all the data relevant to program execution.
//  \bool running -> Marks if the program should continue to run the main loop
//  \ivec2  screenSize -> Current screenSize
//  \SDL_Window *window -> Pointer to the SDL_Window being used
//  \SDL_GLContext context -> The context of SDL
struct ProgramState {
    bool running = true;
    glm::ivec2 screenSize;
    SDL_Window *window;
    SDL_GLContext glContext;
};

inline void crash(char *errorMsg) {
    SDL_Log("ERROR: %s", errorMsg);
    int *ptr = NULL;
    *ptr = 42;
}
