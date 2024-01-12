// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "SDL2/SDL_log.h"

#include "./game.h"

#include "./engine_lib.h"
#include "./entities.h"
#include "./globals.h"
#include "./imgui.h"
#include "./input.h"
#include "./renderer.h"

// NOTE: g is the GlobalState object

// This constant is the target simulations of the world per second
constexpr double UPDATE_DELAY = 1. / 60.;

EntityManager entityManager;
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

    auto playerEntity = std::make_shared<Entity>();
    auto transformComponent =
        std::make_shared<TransformComponent>(glm::vec2(0, 0));
    auto spriteRenderer = std::make_shared<SpriteRenderer>(
        g->renderData, Player, glm::vec2(16, 16), transformComponent);

    playerEntity->components.push_back(transformComponent);
    playerEntity->components.push_back(spriteRenderer);

    entityManager.entities.push_back(playerEntity);
}

static float speed = 1.f;

void draw_imgui_frame(float dt) {
    ImGui::Begin("Hello, world!");
    ImGui::SliderFloat("Player Speed", &speed, 1.0, 5.0);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", dt, 1 / dt);
    ImGui::ColorEdit3("clear color",
                      reinterpret_cast<float *>(g->renderData->clearColor));
    ImGui::End();
}

EXPORT_FN void updateGame(GlobalState *globalStateIn, float dt) {
    // Since this is compiled as a separate dll, it holds its own static data
    if (g != globalStateIn) { g = globalStateIn; }
    if (!g->gameState->initialized) initializeGameState();

    g->gameState->updateTimer += dt;

    // World is simulated every 1/60 seconds
    // https://gafferongames.com/post/fix_your_timestep/
    while (g->gameState->updateTimer >= UPDATE_DELAY) {
        g->gameState->updateTimer -= UPDATE_DELAY;
        g->input->mouseWorldPos = g->input->mousePos / (WORLD_SIZE / TILESIZE);
        simulate();
    }

    draw_imgui_frame(dt);
    entityManager.render();
}

void simulate() {
    // TODO: This is a placeholder
    auto entity = entityManager.entities.at(0);
    auto component = entity->components.at(0);

    // TODO: Implement a way of asking an entity for a specific type of
    // component and an entity manager for an specific entity (maybe ID?)
    if (auto transformComponent =
            std::dynamic_pointer_cast<TransformComponent>(component)) {
        // SDL_Log("The component is a TransformComponent.");
        auto pos = transformComponent->pos;

        if (actionDown(MOVE_UP)) { pos.y -= speed; }
        if (actionDown(MOVE_DOWN)) { pos.y += speed; }
        if (actionDown(MOVE_RIGHT)) { pos.x -= speed; }
        if (actionDown(MOVE_LEFT)) { pos.x += speed; }

        transformComponent->setPos(pos.x, pos.y);
    } else {
        SDL_Log("The component is not a TransformComponent.");
    }

    entityManager.update();
}

// TODO: Implement some kind of InputManager
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
