// Copyright (c) 2024 <Sergio Bermejo de las Heras>main
// This code is subject to the MIT license.

#include "./main.h"

#include "./engine_lib.h"
#include "./globals.h"
#include "./initialization.h"
#include "./input.h"

#include "./fonts.h"
#include "renderer.h"

// NOTE: g is the GlobalState object

global void *gameSO;

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#elif __linux__
int main(int argc, char *args[])
#endif
{
    permStorage = new BumpAllocator(MB(10)), tempStorage = new BumpAllocator(MB(10));
    if (!(g = initialize(permStorage, tempStorage))) { crash("Failed to initialize the engine."); }

    u64 now = SDL_GetPerformanceCounter(), last = 0;
    double dt = 0;

    reloadGameLib(tempStorage);

    // GL_TEXTURE0 is for the font
    load_font("../fonts/joystix.otf", 48, g->renderData, g->glContext, tempStorage);
    loadTextureAtlas("../assets/textures/zelda-like/character.png", g->glContext, GL_TEXTURE1);
    loadTextureAtlas("../assets/textures/zelda-like/objects.png", g->glContext, GL_TEXTURE2);
    loadTextureAtlas("../assets/textures/zelda-like/Overworld.png", g->glContext, GL_TEXTURE3);

    SDL_Event event;
    while (g->appState->running) {
        SDL_ShowCursor(g->input->showCursor);

        last = now;
        now = SDL_GetPerformanceCounter();

        dt = static_cast<double>(now - last) / static_cast<double>(SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
        }

        updateGame_ptr(permStorage, tempStorage, g, dt);
        render(g);
        tempStorage->freeMemory();
        reloadGameLib(tempStorage);

        g->input->prevMouseState[0] = g->input->mouseState[0];
        g->input->prevMouseState[1] = g->input->mouseState[1];
        g->input->prevMouseState[2] = g->input->mouseState[2];
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
        break;
    }

    case SDL_WINDOWEVENT: {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            SDL_GL_GetDrawableSize(g->appState->window, &g->appState->screenSize.x,
                                   &g->appState->screenSize.y);
        }

        break;
    }

    case SDL_WINDOWEVENT_ENTER: {
        g->input->mouseInWindow = true;
        break;
    }

    case SDL_WINDOWEVENT_LEAVE: {
        g->input->mouseInWindow = false;
        break;
    }

    case SDL_MOUSEMOTION: {
        if (g->input->mouseInWindow) {
            SDL_GetMouseState(&g->input->mousePos.x, &g->input->mousePos.y);
            // log(__FILE__, __LINE__,"New mousePos: %d %d", g->input->mousePos.x,
            // g->input->mousePos.y);
        }

        break;
    }

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
        bool pressed = event->type == SDL_MOUSEBUTTONDOWN;

        bool left = event->button.button == SDL_BUTTON_LEFT;
        bool right = event->button.button == SDL_BUTTON_RIGHT;
        bool mid = event->button.button == SDL_BUTTON_MIDDLE;

        if (left) {
            g->input->mouseState[0] = pressed;
        } else if (mid) {
            g->input->mouseState[1] = pressed;
        } else if (right) {
            g->input->mouseState[2] = pressed;
        }

        break;
    }

        // case SDL_KEYUP:
        // case SDL_KEYDOWN: {
        //     break;
        // }
    }
}

void reloadGameLib(BumpAllocator *tempStorage) {
    local_persist u64 lastModTimestamp;

    u64 currentTimestamp = plat_getFileTimestamp(gameSharedObject);
    if (currentTimestamp <= lastModTimestamp) return;

    if (gameSO) {
        if (!plat_freeDynamicLib(gameSO)) crash("Failed to free game.so");

        gameSO = nullptr;
        log("Freed gameSO");
    }

    while (!plat_copyFile(gameSharedObject, loadedgameSharedObject, tempStorage)) {
        if (g->appState->running) platform_sleep(10);
    }
    log("Copied game.so to game_load.so");

    if (!(gameSO = plat_loadDynamicLib(loadedgameSharedObject))) {
        crash("Failed to load game_load.so");
    }

    log("Loaded dynamic library game_load.so");
    if (!(updateGame_ptr = (update_game_type *)plat_loadDynamicFun(gameSO, "updateGame"))) {
        crash("Failed to load updateGame function");
    }

    log("Loaded dynamic function updateGame");
    lastModTimestamp = currentTimestamp;
}
