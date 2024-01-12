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
