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

#ifdef _WIN32
#define gameSharedObject "./game.dll"
#define loadedgameSharedObject "./game_load.dll"
#elif __linux__
#define gameSharedObject "./game.so"
#define loadedgameSharedObject "./game_load.so"
#endif

// I don't think inlining is strictly necessary, but this functions are only
// called here and I mainly extracted them to make this file more
// manageable.
//
// TODO: Test this for performance when I am actually rendering something
// complex
inline void render(int width, int height) {
    auto color = g->renderData->clearColor;
    glClearColor(color[0], color[1], color[2], 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    glm::vec2 screenSize = {static_cast<float>(width),
                            static_cast<float>(height)};
    glUniform2fv(g->glContext->screenSizeID, 1, &screenSize.x);

    glm::mat4x4 mat =
        g->renderData->gameCamera.getProjectionMatrix(width, height);
    glUniformMatrix4fv(g->glContext->orthoProjectionID, 1, GL_FALSE, &mat[0].x);

    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                        sizeof(Transform) * g->renderData->transformCount,
                        g->renderData->transforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6,
                              g->renderData->transformCount);

        g->renderData->transformCount = 0;
    }
}

inline void handleSDLevents(SDL_Event *event) {
    switch (event->type) {
    case SDL_QUIT: {
        g->appState->running = false;
        break;
    }

    case SDL_WINDOWEVENT: {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            SDL_GL_GetDrawableSize(g->appState->window, &g->appState->width,
                                   &g->appState->height);
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
        char const *state = event->type == SDL_MOUSEBUTTONDOWN ? "down" : "up";
        char const *key = event->button.button == SDL_BUTTON_LEFT ? "M1" : "M2";

        // SDL_Log("%s %s", key, state);

        break;
    }

    case SDL_KEYUP:
    case SDL_KEYDOWN: {
        bool pressed = event->type == SDL_KEYDOWN;
        SDL_Keycode keyCode = event->key.keysym.sym;

        // char const *keyName = SDL_GetKeyName(keyCode);
        // SDL_Log("%s '%s'", pressed, keyName);
        //
        // TODO: Right now, the idea is just to update the keys that are
        // relevant (bound) to the game. In the future, I'd like to add states.
        // For example:
        //
        // Text mode: Only update ASCII keycodes.
        // UI mode:   Only update UI keycodes
        updateKeyState(keyCode, pressed, g->input);
        g->input->lastPressed = keyCode;

        break;
    }
    }
}

void updateGame(GlobalState *globalStateIn, double dt) {
    updateGame_ptr(globalStateIn, dt);
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

        while (!plat_copyFile(gameSharedObject, loadedgameSharedObject,
                              tempStorage)) {
            if (g->appState->running) platform_sleep(10);
        }

        SDL_Log("Copied game.so to game_load.so");

        gameSO = plat_loadDynamicLib(loadedgameSharedObject);
        if (!gameSO) crash("Failed to load game_load.so");

        SDL_Log("Loaded dynamic library game_load.so");

        updateGame_ptr =
            (update_game_type *)(plat_loadDynamicFun(gameSO, "updateGame"));

        if (!updateGame_ptr) crash("Failed to load updateGame function");

        SDL_Log("Loaded dynamic function updateGame");

        lastModTimestamp = currentTimestamp;
    }
}

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpCmdLine, int nCmdShow)
#elif __linux__
int main(int argc, char *args[])
#endif
{
    BumpAllocator *permStorage = new BumpAllocator(MB(10));
    BumpAllocator *tempStorage = new BumpAllocator(MB(10));

    // GlobalState just stitches together all pointers
    g = (GlobalState *)permStorage->alloc(sizeof(GlobalState));

    g->appState = (ProgramState *)permStorage->alloc(sizeof(ProgramState));
    g->gameState = (GameState *)permStorage->alloc(sizeof(GameState));
    g->renderData = (RenderData *)permStorage->alloc(sizeof(RenderData));
    g->input = (Input *)permStorage->alloc(sizeof(Input));
    g->imgui = (ImguiState *)permStorage->alloc(sizeof(ImguiState));
    g->glContext = (GLContext *)permStorage->alloc(sizeof(GLContext));

    if (!g->appState || !g->renderData || !g->gameState || !g->input) {
        SDL_Log("ERROR: Failed to alloc globalState");
        return -1;
    }

    g->gameState->initialized = false;

    g->input->mouseInWindow = true;
    g->input->showCursor = true;
    g->input->usedKeys = std::map<SDL_Keycode, KeyState>();

    g->appState->running = true;
    g->appState->width = 1280;
    g->appState->height = 720;
    g->appState->window = NULL;
    g->appState->glContext = NULL;

    g->renderData->clearColor[0] = 119.f / 255.f;
    g->renderData->clearColor[1] = 33.f / 255.f;
    g->renderData->clearColor[2] = 111.f / 255.f;

    if (!initSDLandGL(tempStorage, g->appState, g->glContext, g->renderData)) {
        SDL_Log("ERROR: Failed to initialize SDL or OpenGL!");
        return -1;
    }

    if (!initImgui(g->imgui, g->appState)) {
        SDL_Log("ERROR: Failed to initialize ImGui");
        return -1;
    }

    // Free the memory used for initialization

    // Get initial window size. It should have been initialized to defaults,
    // but who knows
    SDL_GL_GetDrawableSize(g->appState->window, &g->appState->width,
                           &g->appState->height);

    SDL_Event event;
    SDL_StartTextInput();

    loadTextureAtlas("../assets/textures/zelda-like/character.png",
                     g->glContext, GL_TEXTURE0);
    loadTextureAtlas("../assets/textures/zelda-like/objects.png", g->glContext,
                     GL_TEXTURE1);

    u64 now = SDL_GetPerformanceCounter();
    u64 last = 0;
    double dt = 0;

    reloadGameLib(tempStorage);
    while (g->appState->running) {
        last = now;
        now = SDL_GetPerformanceCounter();

        dt = static_cast<double>(now - last) /
             static_cast<double>(SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        SDL_ShowCursor(g->input->showCursor);

        updateGame(g, dt);
        render(g->appState->width, g->appState->height);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(g->appState->window);
        tempStorage->freeMemory();

        reloadGameLib(tempStorage);
    }

    SDL_StopTextInput();
    close(g->glContext, g->appState);

    return 0;
}
