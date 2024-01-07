// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./headers/game.h"
#include "./headers/input.h"
#include "./headers/renderer.h"
#include "headers/engine_lib.h"
#include "imgui.h"

// This constant is the target simulations of the world per second
constexpr double UPDATE_DELAY = 1. / 60.;

inline void initializeGameState() {
    gGameState->updateTimer = 0;
    // Top left is 0, 0
    gGameState->playerPos = {0, 0};

    gRenderData->gameCamera = {
        .pos = {160.f, 90.f},
        .dimensions = {static_cast<float>(WORLD_SIZE.x),
                       static_cast<float>(WORLD_SIZE.y)},
    };

    // Last keycode pressed is stored in gInput->lastPressed. To allow for
    // rebinds the idea right now is that we would check which GameAction is
    // being requested to remap, and, move it to the new (last pressed)
    // keycode
    gameRegisterKey(MOVE_UP, 'w');
    gameRegisterKey(MOVE_RIGHT, 'a');
    gameRegisterKey(MOVE_DOWN, 's');
    gameRegisterKey(MOVE_LEFT, 'd');

    ImGui::SetCurrentContext(gImgui->ctxt);
    ImGui::SetAllocatorFunctions(gImgui->p_alloc_func, gImgui->p_free_func);

    gGameState->initialized = true;
}

EXPORT_FN void updateGame(GameState *gameStateIn, RenderData *renderDataIn,
                          Input *inputIn, ImguiState *imguiIn, float dt) {
    // Since this is compiled as a separate dll, it holds its own static data
    if (gRenderData != renderDataIn) {
        gRenderData = renderDataIn;
        gGameState = gameStateIn;
        gInput = inputIn;
        gImgui = imguiIn;
    }

    if (!gGameState->initialized) initializeGameState();

    {
        static float f = 0.0f;
        static int counter = 0;

        // Create a window called "Hello, world!" and append into it.
        ImGui::Begin("Hello, world!");
        // Display some text (you can use a format strings too)
        ImGui::Text("This is some useful text.");
        // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        // Edit 3 floats representing a color
        ImGui::ColorEdit3("clear color",
                          reinterpret_cast<float *>(gRenderData->clearColor));

        // Buttons return true when clicked (most widgets return true when
        // edited/activated)
        if (ImGui::Button("Button")) counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", dt, 1 / dt);
        ImGui::End();
    }

    gGameState->updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (gGameState->updateTimer >= UPDATE_DELAY) {
        gGameState->updateTimer -= UPDATE_DELAY;

        if (actionDown(MOVE_UP)) { gGameState->playerPos.y -= 2; }
        if (actionDown(MOVE_DOWN)) { gGameState->playerPos.y += 2; }
        if (actionDown(MOVE_RIGHT)) { gGameState->playerPos.x -= 2; }
        if (actionDown(MOVE_LEFT)) { gGameState->playerPos.x += 2; }
        gInput->mouseWorldPos = gInput->mousePos / (WORLD_SIZE / TILESIZE);
    }

    // SDL_Log("FPS: %d, deltaTime: %f, updateDelay: %f", (int)(1 / dt), dt,
    //         UPDATE_DELAY);

    draw_sprite(gRenderData, Player, gGameState->playerPos, {16.f, 16.f});
}
