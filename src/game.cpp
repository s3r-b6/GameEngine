// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./imgui.h"
#include "SDL2/SDL_log.h"

#include "./game.h"

#include "./engine_lib.h"
#include "./entities.h"
#include "./globals.h"
#include "./input.h"
#include "./renderer.h"

// NOTE: g is the GlobalState object

// This constant is the target simulations of the world per second
global float speed = 1.f;
global glm::ivec2 lastScreenSize;

EntityManager entityManager;
TileManager tileManager;

Tile t1 = {Grass1, 2};
Tile t2 = {Water1, 2};

inline void initializeGameState() {
    lastScreenSize = g->appState->screenSize;

    g->gameState->updateTimer = 0;
    g->renderData->gameCamera.pos = {320.f, 180.f};

    g->renderData->gameCamera.dimensions = {WORLD_SIZE.x, WORLD_SIZE.y};

    gameRegisterKey(MOVE_UP, 'w');
    gameRegisterKey(MOVE_RIGHT, 'a');
    gameRegisterKey(MOVE_DOWN, 's');
    gameRegisterKey(MOVE_LEFT, 'd');

    ImGui::SetCurrentContext(g->imgui->ctxt);
    ImGui::SetAllocatorFunctions(g->imgui->p_alloc_func, g->imgui->p_free_func);

    auto playerEntity = std::make_shared<Entity>();
    auto transformComponent = std::make_shared<TransformComponent>(glm::vec2(0, 0));
    auto spriteRenderer = std::make_shared<SpriteRenderer>(g->renderData, Player, glm::vec2(16, 32),
                                                           transformComponent);

    playerEntity->components.push_back(transformComponent);
    playerEntity->components.push_back(spriteRenderer);

    entityManager.entities.push_back(playerEntity);
    g->gameState->initialized = true;
}

void draw_imgui_frame(float dt) {
    ImGui::Begin("Hello, world!");
    ImGui::SliderFloat("Player Speed", &speed, 1.0, 5.0);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", dt, 1 / dt);
    ImGui::ColorEdit3("clear color", reinterpret_cast<float *>(g->renderData->clearColor));
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

        g->input->mouseWorldPos = g->renderData->gameCamera.getMousePosInWorld(
            g->input->mousePos, g->appState->screenSize);

        if (g->input->mouseInWindow && g->input->mLeftDown) {
            auto pos = g->input->mouseWorldPos;
            tileManager.setTile(pos.x, pos.y, &t1);
            // SDL_Log("Placing tile at %d %d", pos.x, pos.y);
        }

        if (g->input->mouseInWindow && g->input->mRightDown) {
            auto pos = g->input->mouseWorldPos;
            tileManager.removeTile(pos.x, pos.y);
            // SDL_Log("Removing tile at %d %d", pos.x, pos.y);
        }

        simulate();
    }

    draw_imgui_frame(dt);
    entityManager.render();
    tileManager.render();

    lastScreenSize = g->appState->screenSize;
}

void simulate() {
    // TODO: This is a placeholder
    auto entity = entityManager.entities.at(0);
    auto component = entity->components.at(0);

    // TODO: Implement a way of asking an entity for a specific type of
    // component and an entity manager for an specific entity (maybe ID?)
    if (auto transformComponent = std::dynamic_pointer_cast<TransformComponent>(component)) {
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
