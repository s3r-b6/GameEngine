// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./main.h"
#include "./engine_lib.h"
#include "./globals.h"
#include "./initialization.h"
#include "./input.h"
#include "./renderer.h"

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_video.h"

#ifdef _WIN32
#define gameSharedObject "./game.dll"
#define loadedgameSharedObject "./game_load.dll"
#elif __linux__
#define gameSharedObject "./game.so"
#define loadedgameSharedObject "./game_load.so"
#endif

// NOTE: g is the GlobalState object

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
            // SDL_Log("New mousePos: %d %d", g->input->mousePos.x,
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

        if (left) g->input->mLeftDown = pressed;
        if (mid) g->input->mMidDown = pressed;
        if (right) g->input->mRightDown = pressed;

        break;
    }

    case SDL_KEYUP:
    case SDL_KEYDOWN: {
        bool pressed = event->type == SDL_KEYDOWN;
        SDL_Keycode keyCode = event->key.keysym.sym;

        updateKeyState(keyCode, pressed, g->input);
        g->input->lastPressed = keyCode;

        break;
    }
    }
}

void reloadGameLib(BumpAllocator *tempStorage) {
    local_persist void *gameSO;
    local_persist u64 lastModTimestamp;

    u64 currentTimestamp = plat_getFileTimestamp(gameSharedObject);

    if (currentTimestamp > lastModTimestamp) {
        if (gameSO) {
            bool freeResult = plat_freeDynamicLib(gameSO);

            if (!freeResult) crash("Failed to free game.so");

            gameSO = nullptr;
            SDL_Log("Freed gameSO");
        }

        while (!plat_copyFile(gameSharedObject, loadedgameSharedObject, tempStorage)) {
            if (g->appState->running) platform_sleep(10);
        }

        SDL_Log("Copied game.so to game_load.so");

        gameSO = plat_loadDynamicLib(loadedgameSharedObject);
        if (!gameSO) crash("Failed to load game_load.so");

        SDL_Log("Loaded dynamic library game_load.so");

        updateGame_ptr = (update_game_type *)(plat_loadDynamicFun(gameSO, "updateGame"));

        if (!updateGame_ptr) crash("Failed to load updateGame function");

        SDL_Log("Loaded dynamic function updateGame");

        lastModTimestamp = currentTimestamp;
    }
}

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#elif __linux__
int main(int argc, char *args[])
#endif
{
    if (!(g = initialize())) {
        SDL_Log("Failed to initialize the engine.");
        return -1;
    }

    u64 now = SDL_GetPerformanceCounter(), last = 0;
    double dt = 0;

    reloadGameLib(tempStorage);

    // TODO: This should be handled by some kind of system
    loadTextureAtlas("../assets/textures/zelda-like/character.png", g->glContext, GL_TEXTURE0);
    loadTextureAtlas("../assets/textures/zelda-like/objects.png", g->glContext, GL_TEXTURE1);
    loadTextureAtlas("../assets/textures/zelda-like/Overworld.png", g->glContext, GL_TEXTURE2);

    SDL_Event event;
    while (g->appState->running) {
        SDL_ShowCursor(g->input->showCursor);

        last = now;
        now = SDL_GetPerformanceCounter();

        dt = static_cast<double>(now - last) / static_cast<double>(SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
        }

        updateGame_ptr(g, dt);
        render(g);
        tempStorage->freeMemory();
        reloadGameLib(tempStorage);
    }

    close(g->glContext, g->appState);
    return 0;
}
