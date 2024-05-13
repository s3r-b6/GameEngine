#pragma once

#include "types.h"

// Append to the shaders location the file
#define SHADER_SRC(termination) "../assets/shaders/" termination

void printProgramLog(uint program, BumpAllocator *tempStorage);
void printShaderLog(uint shader, BumpAllocator *tempStorage);
bool loadShaders(char *vertShaderPath, char *fragShaderPath, BumpAllocator *tempStorage);
