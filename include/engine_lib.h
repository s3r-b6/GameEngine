#pragma once

#include "./types.h"

#include "SDL2/SDL_log.h"
#include "SDL2/SDL_video.h"

#include <string>

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

#define log(_msg, ...) _log(__FILE__, __LINE__, _msg);
#define crash(_msg, ...) _log(__FILE__, __LINE__, _msg);

void _log(const std::string filename, const std::uint_fast32_t line, const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, msg, argsCopy) + 1;
    va_end(argsCopy);

    std::string result(size, '\0');
    vsnprintf(&result[0], size, msg, args);

    printf("[ INFO ] (%s : %lu)\n", filename.c_str(), line);
    printf("  ==> %s\n", result.c_str());
}

void _crash(const std::string filename, const std::uint_fast32_t line, const char *errorMsg, ...) {
    va_list args;
    va_start(args, errorMsg);

    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, errorMsg, argsCopy) + 1;
    va_end(argsCopy);

    std::string result(size, '\0');
    vsnprintf(&result[0], size, errorMsg, args);

    printf("[ FATAL ERROR: ] %s, %lu\n", filename.c_str(), line);
    printf("  ==> %s\n", result.c_str());

    int *ptr = NULL;
    *ptr = 42;
}
