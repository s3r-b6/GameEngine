#pragma once

#include "./types.h"

struct RenderData;
struct GLContext;
struct ImguiState;
struct ProgramState;
struct GameState;
struct Input;

struct GlobalState {
    RenderData *renderData;
    GLContext *glContext;
    ImguiState *imgui;
    ProgramState *appState;
    GameState *gameState;
    Input *input;
};

global GlobalState *g;

constexpr int MAX_TEXTURES = 8;
constexpr double UPDATE_DELAY = 1. / 60.;
constexpr int MAX_TRANSFORMS = 256;
constexpr glm::ivec2 WORLD_SIZE = {640, 360};
constexpr int TILESIZE = 16;
