#pragma once

#include "./types.h"

#include "SDL2/SDL_log.h"
#include "SDL2/SDL_video.h"

#include <stdarg.h>
#include <stdio.h>
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

#define log(_msg...) _log(__FILE__, __LINE__, _msg);
#define crash(_msg...) _crash(__FILE__, __LINE__, _msg);

#define DEBUG_FORMAT __attribute__((format(__printf__, 3, 4)))

DEBUG_FORMAT inline void _log(const std::string &filename, const u64 line, const char *fmt...) {

    va_list args, argsCopy;
    va_start(args, fmt);

    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, fmt, argsCopy) + 1;
    va_end(argsCopy);

    if (size < 0) { return; }

    std::string result(size, '\0');
    vsnprintf(&result[0], size, fmt, args);

    printf("[ \u001b[34mINFO %s : %lu \u001b[37m] ", filename.c_str(), line);
    printf(" %s\n", result.c_str());

    va_end(args);
}

DEBUG_FORMAT inline void _crash(const std::string &filename, const std::uint_fast32_t line,
                                const char *fmt...) {
    va_list args, argsCopy;
    va_start(args, fmt);

    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, fmt, argsCopy) + 1;
    va_end(argsCopy);

    if (size < 0) { return; }

    std::string result(size, '\0');
    vsnprintf(&result[0], size, fmt, args);

    printf("[ \u001b[31mFATAL ERROR:  %s : %lu \u001b[37m] ", filename.c_str(), line);
    printf(" %s\n", result.c_str());

    va_end(args);

    int *ptr = NULL;
    *ptr = 42;
}
