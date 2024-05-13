#pragma once

#include "./types.h"
#include <SDL2/SDL.h>

void initialize(BumpAllocator *permStorage, BumpAllocator *tempStorage);
bool initSDLandGL(BumpAllocator *tempStorage);
bool initGL(BumpAllocator *tempStorage);
void close();

void printProgramLog(uint program, BumpAllocator *tempStorage);
void printShaderLog(uint shader, BumpAllocator *tempStorage);
bool initAudio();
