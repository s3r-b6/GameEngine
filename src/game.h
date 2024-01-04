#pragma once

#include "assets.h"
#include "engine_lib.h"
#include "renderer.h"

#include "platform.h"

// TODO: In win32 we need __declspec(dllexport)
extern "C" {
EXPORT_FN void updateGame(RenderData *renderDataIn);
}
