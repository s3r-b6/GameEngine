// Copyright (c) 2024 <Sergio Bermejo de las Heras>main
// This code is subject to the MIT license.

#include "./main.h"

#include "./engine_global.h"
#include "./engine_lib.h"
#include "./initialization.h"
#include "./input.h"

#include "./audio.h"
#include "./fonts.h"
#include "./renderer.h"

// NOTE: g is the GlobalState object

global void *gameSO;

int plat_main() {
    permStorage = new BumpAllocator(MB(10)), tempStorage = new BumpAllocator(MB(10));
    if (!(g = initialize(permStorage, tempStorage))) { crash("Failed to initialize the engine."); }

    u64 now = SDL_GetPerformanceCounter(), last = 0;
    double dt = 0;

    reloadGameLib(tempStorage);

    // GL_TEXTURE0 is for the font
    load_font("../fonts/joystix.otf", 48, g->renderData, g->glContext, tempStorage);
    loadTextureAtlas("../assets/textures/zelda-like/character.png", g->glContext, GL_TEXTURE1,
                     false);
    loadTextureAtlas("../assets/textures/zelda-like/objects.png", g->glContext, GL_TEXTURE2, true);
    loadTextureAtlas("../assets/textures/zelda-like/Overworld.png", g->glContext, GL_TEXTURE3,
                     true);

    SDL_Event event;

    local_persist float hotreload_timer = 3.f;

    while (g->appState->running) {
        SDL_ShowCursor(g->input->showCursor);

        g->input->mouseState = g->input->mouseState | g->input->mouseState << 4;

        last = now;
        now = SDL_GetPerformanceCounter();

        dt = static_cast<double>(now - last) / static_cast<double>(SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
        }

        updateGame_ptr(permStorage, tempStorage, g, dt);

        auto screenSize = g->appState->screenSize;
        auto color = g->renderData->clearColor;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(color[0], color[1], color[2], 1.f);
        glClearDepth(0.f);
        glViewport(0, 0, screenSize.x, screenSize.y);

        render(g);
        glFinish();
        ui_render(g);
        SDL_GL_SwapWindow(g->appState->window);

        tempStorage->freeMemory();

        hotreload_timer -= dt;
        if (hotreload_timer <= 0.f) {
            reloadGameLib(tempStorage);
            hotreload_timer = 3.f;
        }

        if ((g->input->mouseState & 0x0F) == 0) {
            g->input->mouseState = g->input->mouseState << 4;
        }
    }

    close(g->glContext, g->appState, g->alState);
    plat_freeDynamicLib(gameSO);
    plat_deleteFile(loadedgameSharedObject);
    return 0;
}

inline void handleSDLevents(SDL_Event *event) {
    switch (event->type) {
    case SDL_QUIT: {
        g->appState->running = false;
    } break;

    case SDL_WINDOWEVENT: {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            SDL_GL_GetDrawableSize(g->appState->window, &g->appState->screenSize.x,
                                   &g->appState->screenSize.y);
        }
        break;
    }

    case SDL_WINDOWEVENT_ENTER: {
        g->input->mouseInWindow = true;
    } break;

    case SDL_WINDOWEVENT_LEAVE: {
        g->input->mouseInWindow = false;
    } break;

    case SDL_MOUSEMOTION: {
        if (g->input->mouseInWindow) {
            SDL_GetMouseState(&g->input->mousePos.x, &g->input->mousePos.y);
            // log(__FILE__, __LINE__,"New mousePos: %d %d", g->input->mousePos.x,
            // g->input->mousePos.y);
        }
    } break;

    case SDL_MOUSEBUTTONUP: {
        if (event->button.button == SDL_BUTTON_LEFT) {
            g->input->mouseState &= ~(0x1);
            g->input->mouseState |= (0x1 << 4);
        } else if (event->button.button == SDL_BUTTON_MIDDLE) {
            g->input->mouseState &= ~(0x1 << 1);
            g->input->mouseState |= (0x1 << 5);
        } else if (event->button.button == SDL_BUTTON_RIGHT) {
            g->input->mouseState &= ~(0x1 << 2);
            g->input->mouseState |= (0x1 << 6);
        }
    } break;

    case SDL_MOUSEBUTTONDOWN: {
        if (event->button.button == SDL_BUTTON_LEFT) {
            g->input->mouseState |= 0x1;
            g->input->mouseState &= ~(0x1 << 4);
        } else if (event->button.button == SDL_BUTTON_MIDDLE) {
            g->input->mouseState |= (0x1 << 1);
            g->input->mouseState &= ~(0x1 << 5);
        } else if (event->button.button == SDL_BUTTON_RIGHT) {
            g->input->mouseState |= (0x1 << 2);
            g->input->mouseState &= ~(0x1 << 6);
        }
    }

    break;
    }
}

void reloadGameLib(BumpAllocator *tempStorage) {
    local_persist u64 lastModTimestamp;

    u64 currentTimestamp = plat_getFileTimestamp(gameSharedObject);
    if (g->appState->running && currentTimestamp == 0) platform_sleep(350);

    if (currentTimestamp <= lastModTimestamp) return;

    if (gameSO) {
        if (!plat_freeDynamicLib(gameSO)) crash("Failed to free game.so");

        gameSO = nullptr;
        engine_log("Freed gameSO");
    }

    while (!plat_copyFile(gameSharedObject, loadedgameSharedObject, tempStorage)) {
        if (g->appState->running) platform_sleep(50);
    }

    engine_log("Copied game.so to game_load.so");

    if (!(gameSO = plat_loadDynamicLib(loadedgameSharedObject))) {
        crash("Failed to load game_load.so");
    }

    engine_log("Loaded dynamic library game_load.so");
    if (!(updateGame_ptr = (update_game_type *)plat_loadDynamicFun(gameSO, "updateGame"))) {
        crash("Failed to load updateGame function");
    }

    engine_log("Loaded dynamic function updateGame");
    lastModTimestamp = currentTimestamp;
}
