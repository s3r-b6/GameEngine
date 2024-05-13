#pragma once

#include "types.h"
void printShaderLog(uint shader, BumpAllocator *tempStorage);
bool loadShaders(char *vertShaderPath, char *fragShaderPath, uint &vertexShaderID,
                 uint &fragmentShaderID, BumpAllocator *tempStorage);
