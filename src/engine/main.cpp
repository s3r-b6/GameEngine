// Copyright (c) 2024 <Sergio Bermejo de las Heras>main
// This code is subject to the MIT license.

#include "./main.h"

#include "./engine_global.h"
#include "./engine_lib.h"

#include "./fonts.h"
#include "./initialization.h"
#include "./input.h"
#include "./renderer.h"
#include "engine_memory.h"

_global void *gameSO;

int plat_main() {
    permStorage = new BumpAllocator(MB(10)), tempStorage = new BumpAllocator(MB(5));

    if (!initialize(permStorage, tempStorage)) { crash("Failed to initialize the engine."); }

    u64 now = SDL_GetPerformanceCounter(), last = 0;
    double dt = 0;

    reloadGameLib(tempStorage);

    // GL_TEXTURE0 is for the font
    load_font("../fonts/joystix.otf", 48, tempStorage);

    loadTextureAtlas("../assets/textures/zelda-like/Overworld.png", GL_TEXTURE1, true);
    loadTextureAtlas("../assets/textures/zelda-like/objects.png", GL_TEXTURE2, true);
    loadTextureAtlas("../assets/textures/zelda-like/character.png", GL_TEXTURE3, false);

    SDL_Event event;
    local_persist float hotreload_timer = 1.5f;
    while (appState->running) {
        // SDL_ShowCursor(input->showCursor);
        input->mouseState = input->mouseState | input->mouseState << 4;

        last = now, now = SDL_GetPerformanceCounter();
        dt = static_cast<double>(now - last) / static_cast<double>(SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event) != 0) { handleSDLevents(&event); }

        updateGame_ptr(permStorage, tempStorage, renderData, appState, gameState, glContext, input,
                       dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(renderData->clearColor[0], renderData->clearColor[1],
                     renderData->clearColor[2], 1.f);
        glClearDepth(0.f);
        glViewport(0, 0, appState->screenSize.x, appState->screenSize.y);

        render();
        glFinish();
        UIrender();
        SDL_GL_SwapWindow(appState->window);

        input->mouseState &= ~(input->mouseState & 0xF0);

        hotreload_timer -= dt;
        // Try to load a new shared object every x seconds
        if (hotreload_timer <= 0.f) {
            reloadGameLib(tempStorage);
            reloadShaderCode(tempStorage);
            hotreload_timer = 4.f;
        }
        tempStorage->freeMemory();
    }

    close();
    plat_freeDynamicLib(gameSO);
    plat_deleteFile(loadedgameSharedObject);
    return 0;
}

inline void handleSDLevents(SDL_Event *event) {
    switch (event->type) {
    case SDL_QUIT: {
        appState->running = false;
    } break;

    case SDL_WINDOWEVENT: {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            SDL_GL_GetDrawableSize(appState->window, &appState->screenSize.x,
                                   &appState->screenSize.y);
        }
        break;
    }

    case SDL_WINDOWEVENT_ENTER: {
        input->mouseInWindow = true;
    } break;

    case SDL_WINDOWEVENT_LEAVE: {
        input->mouseInWindow = false;
    } break;

    case SDL_MOUSEMOTION: {
        if (input->mouseInWindow) {
            SDL_GetMouseState(&input->mousePos.x, &input->mousePos.y);
            // log(__FILE__, __LINE__,"New mousePos: %d %d", input->mousePos.x,
            // input->mousePos.y);
        }
    } break;

    case SDL_MOUSEBUTTONUP: {
        if (event->button.button == SDL_BUTTON_LEFT) {
            input->mouseState &= ~(0x1);
            input->mouseState |= (0x1 << 4);
        } else if (event->button.button == SDL_BUTTON_MIDDLE) {
            input->mouseState &= ~(0x1 << 1);
            input->mouseState |= (0x1 << 5);
        } else if (event->button.button == SDL_BUTTON_RIGHT) {
            input->mouseState &= ~(0x1 << 2);
            input->mouseState |= (0x1 << 6);
        }
    } break;

    case SDL_MOUSEBUTTONDOWN: {
        if (event->button.button == SDL_BUTTON_LEFT) {
            input->mouseState |= 0x1;
            input->mouseState &= ~(0x1 << 4);
        } else if (event->button.button == SDL_BUTTON_MIDDLE) {
            input->mouseState |= (0x1 << 1);
            input->mouseState &= ~(0x1 << 5);
        } else if (event->button.button == SDL_BUTTON_RIGHT) {
            input->mouseState |= (0x1 << 2);
            input->mouseState &= ~(0x1 << 6);
        }
    }

    break;
    }
}

void reloadShaderCode(BumpAllocator *tempStorage) {}

void reloadGameLib(BumpAllocator *tempStorage) {
    local_persist u64 lastModTimestamp;

    u64 currentTimestamp = plat_getFileTimestamp(gameSharedObject);
    if (appState->running && currentTimestamp == 0) platform_sleep(350);

    if (currentTimestamp <= lastModTimestamp) return;

    if (gameSO) {
        updateGame_ptr = nullptr;
        if (!plat_freeDynamicLib(gameSO)) crash("Failed to free game.so");

        gameSO = nullptr;
        engine_log("Freed old gameSO");
    }

    if (!plat_deleteFile(loadedgameSharedObject)) {
        crash("Failed to delete %s", loadedgameSharedObject);
    }

    while (!plat_copyFile(gameSharedObject, loadedgameSharedObject, tempStorage)) {
        if (appState->running) platform_sleep(50);
    }

    gameSO = plat_loadDynamicLib(loadedgameSharedObject);
    if (!gameSO) { crash("Failed to load game_load.so"); }
    engine_log("Loaded new gameSO: %p", gameSO);

    updateGame_ptr = (update_game_type *)plat_loadDynamicFun(gameSO, "updateGame");
    if (!updateGame_ptr) { crash("Failed to load updateGame function"); }
    engine_log("Loaded new updateGame_ptr: %p", updateGame_ptr);

    lastModTimestamp = currentTimestamp;
}
