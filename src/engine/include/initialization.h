#pragma once

#include "./types.h"
#include <SDL2/SDL.h>

bool initialize(BumpAllocator *permStorage, BumpAllocator *tempStorage);
inline bool initSDLandGL(BumpAllocator *tempStorage);
inline bool initGL(BumpAllocator *tempStorage);
void close();

void printProgramLog(uint program, BumpAllocator *tempStorage);
void printShaderLog(uint shader, BumpAllocator *tempStorage);

bool initOpenAL();
void exitOpenAL();
