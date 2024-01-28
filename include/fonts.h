#pragma once

#include "./types.h"

void load_font(char *filePath, int fontSize, RenderData *renderData, GLContext *glContext,
               BumpAllocator *tempStorage);
