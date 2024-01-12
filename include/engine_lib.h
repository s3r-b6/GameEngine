#pragma once

#include "./types.h"
#include "imgui.h"

#include "SDL2/SDL_log.h"
#include "SDL2/SDL_video.h"

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
    glm::ivec2 screenSize;
    SDL_Window *window;
    SDL_GLContext glContext;
};

struct ImguiState {
    ImGuiContext *ctxt;
    ImGuiMemAllocFunc p_alloc_func;
    ImGuiMemFreeFunc p_free_func;
    void *p_user_data;
};

inline void crash(char *errorMsg) {
    SDL_Log("ERROR: %s", errorMsg);
    int *ptr = NULL;
    *ptr = 42;
}
