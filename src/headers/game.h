#pragma once

#include "platform.h"
#include "renderer.h"

// TODO: In win32 we need __declspec(dllexport)
extern "C" {
EXPORT_FN void updateGame(RenderData *renderDataIn);
}
