#include "initialization.h"

#include "engine_lib.h"
#include "renderer.h"

#include "platform.h"

// TODO: This should be load as a shared object in linux and as a dll in win32
#include "game.h"

// Game shared object stub
typedef decltype(updateGame) update_game_type;
static update_game_type *updateGame_ptr;

void updateGame(RenderData *renderDataIn);
void reloadGame(BumpAllocator *transientStorage);

// I don't think inlining is strictly necessary, but this functions are only
// called here and I mainly extracted them to make this file more
// manageable.
//
// TODO: Test this for performance when I am actually rendering something
// complex
inline void render() {
    glClearColor(119.f / 255.f, 33.f / 255.f, 111.f / 255.f, 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, gAppState->width, gAppState->height);

    glm::vec2 screenSize = {(float)gAppState->width, (float)gAppState->height};
    glUniform2fv(gGlContext.screenSizeID, 1, &screenSize.x);

    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                        sizeof(Transform) * gRenderData->transformCount,
                        gRenderData->transforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, gRenderData->transformCount);

        gRenderData->transformCount = 0;
    }
}

inline void handleSDLevents(SDL_Event *event) {
    switch (event->type) {
    case SDL_QUIT: {
        gAppState->running = false;
        break;
    }

    case SDL_WINDOWEVENT: {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            SDL_GL_GetDrawableSize(gAppState->window, &gAppState->width,
                                   &gAppState->height);
        }

        break;
    }

    case SDL_TEXTINPUT: {
        int x = 0, y = 0;
        SDL_GetMouseState(&x, &y);

        char key = event->text.text[0];
        SDL_Log("Pressed '%c'", key);
        break;
    }
    }
}

int main(int argc, char *args[]) {
    BumpAllocator *permanentStorage = new BumpAllocator(MB(10));
    BumpAllocator *transientStorage = new BumpAllocator(MB(10));

    gAppState = (ProgramState *)permanentStorage->alloc(sizeof(ProgramState));
    gRenderData = (RenderData *)permanentStorage->alloc(sizeof(RenderData));

    gAppState->running = true;
    gAppState->width = 1280;
    gAppState->height = 720;
    gAppState->window = NULL;
    gAppState->glContext = NULL;

    if (!gAppState || !gRenderData) {
        SDL_Log("ERROR: Failed to alloc ProgramState* or RenderData*");
        return -1;
    }

    if (!initSDLandGL(gAppState, &gGlContext, gRenderData, transientStorage)) {
        SDL_Log("ERROR: Failed to initialize SDL or OpenGL!");
        return -1;
    }

    // Free the memory used for initialization

    // Get initial window size. It should have been initialized to defaults,
    // but who knows
    SDL_GL_GetDrawableSize(gAppState->window, &gAppState->width,
                           &gAppState->height);

    SDL_Event event;
    SDL_StartTextInput();

    loadTextureAtlas("../assets/textures/zelda-like/character.png", &gGlContext,
                     GL_TEXTURE0);
    loadTextureAtlas("../assets/textures/zelda-like/objects.png", &gGlContext,
                     GL_TEXTURE1);

    reloadGame(transientStorage);
    while (gAppState->running) {
        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
        }

        updateGame(gRenderData);
        render();
        SDL_GL_SwapWindow(gAppState->window);
        transientStorage->freeMemory();

        reloadGame(transientStorage);
    }

    SDL_StopTextInput();
    close(gAppState, &gGlContext);
    return 0;
}

void updateGame(RenderData *renderDataIn) { updateGame_ptr(renderDataIn); }

void reloadGame(BumpAllocator *transientStorage) {
    local_persist void *gameSO;
    local_persist u64 lastModTimestamp;

    u64 currentTimestamp = plat_getFileTimestamp("./game.so");

    if (currentTimestamp > lastModTimestamp) {
        SDL_Log("Current sharedObject is newer");
        if (gameSO) {
            bool freeResult = plat_freeDynamicLib(gameSO);

            if (!freeResult) crash("Failed to free game.so");

            gameSO = nullptr;
            SDL_Log("Freed gameSO");
        }

        while (
            !transientStorage->plat_copyFile("./game.so", "./game_load.so")) {
            if (gAppState->running) sleep(1);
        }

        SDL_Log("Copied game.so to game_load.so");

        gameSO = plat_loadDynamicLib("./game_load.so");
        if (!gameSO) crash("Failed to load game_load.so");

        SDL_Log("Loaded dynamic library game_load.so");

        updateGame_ptr =
            (update_game_type *)plat_loadDynamicFun(gameSO, "updateGame");
        if (!updateGame_ptr) crash("Failed to load updateGame function");

        SDL_Log("Loaded dynamic function updateGame");

        lastModTimestamp = currentTimestamp;
    }
}
