// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "SDL2/SDL_log.h"

#include "./engine_lib.h"
#include "./game.h"
#include "./globals.h"
#include "./imgui.h"
#include "./input.h"
#include "./renderer.h"

// This constant is the target simulations of the world per second
constexpr double UPDATE_DELAY = 1. / 60.;

inline void initializeGameState() {

    g->gameState->updateTimer = 0;
    // Top left is 0, 0
    g->gameState->playerPos = {0, 0};

    g->renderData->gameCamera = {
        .pos = {160.f, 90.f},
        .dimensions = {static_cast<float>(WORLD_SIZE.x),
                       static_cast<float>(WORLD_SIZE.y)},
    };

    // Last keycode pressed is stored in g->input->lastPressed. To allow for
    // rebinds the idea right now is that we would check which GameAction is
    // being requested to remap, and, move it to the new (last pressed)
    // keycode
    gameRegisterKey(MOVE_UP, 'w');
    gameRegisterKey(MOVE_RIGHT, 'a');
    gameRegisterKey(MOVE_DOWN, 's');
    gameRegisterKey(MOVE_LEFT, 'd');

    ImGui::SetCurrentContext(g->imgui->ctxt);
    ImGui::SetAllocatorFunctions(g->imgui->p_alloc_func, g->imgui->p_free_func);

    g->gameState->initialized = true;
}

EXPORT_FN void updateGame(GlobalState *globalStateIn, float dt) {
    // Since this is compiled as a separate dll, it holds its own static data
    if (g != globalStateIn) { g = globalStateIn; }
    if (!g->gameState->initialized) initializeGameState();

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
                          reinterpret_cast<float *>(g->renderData->clearColor));

        // Buttons return true when clicked (most widgets return true when
        // edited/activated)
        if (ImGui::Button("Button")) counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", dt, 1 / dt);
        ImGui::End();
    }

    g->gameState->updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (g->gameState->updateTimer >= UPDATE_DELAY) {
        g->gameState->updateTimer -= UPDATE_DELAY;

        if (actionDown(MOVE_UP)) { g->gameState->playerPos.y -= 1; }
        if (actionDown(MOVE_DOWN)) { g->gameState->playerPos.y += 1; }
        if (actionDown(MOVE_RIGHT)) { g->gameState->playerPos.x -= 1; }
        if (actionDown(MOVE_LEFT)) { g->gameState->playerPos.x += 1; }
        g->input->mouseWorldPos = g->input->mousePos / (WORLD_SIZE / TILESIZE);
    }

    // SDL_Log("FPS: %d, deltaTime: %f, updateDelay: %f", (int)(1 / dt), dt,
    //         UPDATE_DELAY);

    draw_sprite(g->renderData, Player, g->gameState->playerPos, {16.f, 16.f});
}

inline bool gameRegisterKey(GameAction action, SDL_Keycode kc) {
    g->gameState->gameBinds[action] = kc;
    if (registerKey(kc, g->input)) {
        SDL_Log("Succesfuly bound %c to game action: %d", kc, action);
        return true;
    } else {
        SDL_Log("Could not bind %c to game action: %d", kc, action);
        return false;
    }
}

inline bool actionDown(GameAction action) {
    if (!g->gameState->gameBinds[action]) return false;

    KeyState ks = {};
    getKeyState(g->gameState->gameBinds[action], &ks, g->input);

    return ks.isDown;
}

inline bool actionUp(GameAction action) {
    if (!g->gameState->gameBinds[action]) return false;

    KeyState ks = {};
    getKeyState(g->gameState->gameBinds[action], &ks, g->input);

    return !ks.isDown;
}
