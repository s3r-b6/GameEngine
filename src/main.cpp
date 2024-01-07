#include "./headers/main.h"
#include "./headers/input.h"
#include "./headers/renderer.h"

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
inline void render() {
    glClearColor(119.f / 255.f, 33.f / 255.f, 111.f / 255.f, 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, gAppState->width, gAppState->height);

    glm::vec2 screenSize = {
        static_cast<float>(gAppState->width),
        static_cast<float>(gAppState->height),
    };
    glUniform2fv(gGlContext.screenSizeID, 1, &screenSize.x);

    glm::mat4x4 mat = gRenderData->gameCamera.getProjectionMatrix();

    glUniformMatrix4fv(gGlContext.orthoProjectionID, 1, GL_FALSE,
                       glm::value_ptr(mat));

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

    case SDL_WINDOWEVENT_ENTER: {
        gInput->mouseInWindow = true;
        break;
    }

    case SDL_WINDOWEVENT_LEAVE: {
        gInput->mouseInWindow = false;
        break;
    }

    case SDL_MOUSEMOTION: {
        if (gInput->mouseInWindow) {
            SDL_GetMouseState(&gInput->mousePos.x, &gInput->mousePos.y);
            // SDL_Log("New mousePos: %d %d", gInput->mousePos.x,
            // gInput->mousePos.y);
        }

        break;
    }

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
        char const *state = event->type == SDL_MOUSEBUTTONDOWN ? "down" : "up";

        char const *key = event->button.button == SDL_BUTTON_LEFT ? "M1" : "M2";

        SDL_Log("%s %s", key, state);

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
        updateKeyState(keyCode, pressed);
        gInput->lastPressed = keyCode;

        break;
    }
    }
}

void updateGame(GameState *gameStateIn, RenderData *renderDataIn,
                Input *inputIn, double dt) {
    updateGame_ptr(gameStateIn, renderDataIn, inputIn, dt);
}

void reloadGameLib(BumpAllocator *transientStorage) {
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
                              transientStorage)) {
            if (gAppState->running) platform_sleep(10);
        }

        SDL_Log("Copied game.so to game_load.so");

        gameSO = plat_loadDynamicLib(loadedgameSharedObject);
        if (!gameSO) crash("Failed to load game_load.so");

        SDL_Log("Loaded dynamic library game_load.so");

        updateGame_ptr = reinterpret_cast<update_game_type *>(
            plat_loadDynamicFun(gameSO, "updateGame"));

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
    BumpAllocator *permanentStorage = new BumpAllocator(MB(10));
    BumpAllocator *transientStorage = new BumpAllocator(MB(10));

    gAppState = reinterpret_cast<ProgramState *>(
        permanentStorage->alloc(sizeof(ProgramState)));
    gGameState = reinterpret_cast<GameState *>(
        permanentStorage->alloc(sizeof(GameState)));
    gRenderData = reinterpret_cast<RenderData *>(
        permanentStorage->alloc(sizeof(RenderData)));
    gInput = reinterpret_cast<Input *>(permanentStorage->alloc(sizeof(Input)));

    if (!gAppState || !gRenderData || !gGameState || !gInput) {
        SDL_Log("ERROR: Failed to alloc ProgramState*, RenderData*, GameState* "
                "or Input*");
        return -1;
    }

    gGameState->initialized = false;

    gInput->mouseInWindow = true;
    gInput->showCursor = true;
    gInput->usedKeys = std::map<SDL_Keycode, KeyState>();

    gAppState->running = true;
    gAppState->width = 1280;
    gAppState->height = 720;
    gAppState->window = NULL;
    gAppState->glContext = NULL;

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

    u64 now = SDL_GetPerformanceCounter();
    u64 last = 0;
    double dt = 0;

    reloadGameLib(transientStorage);
    while (gAppState->running) {
        last = now;
        now = SDL_GetPerformanceCounter();

        dt = static_cast<double>(now - last) /
             static_cast<double>(SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
        }

        SDL_ShowCursor(gInput->showCursor);

        updateGame(gGameState, gRenderData, gInput, dt);
        render();
        SDL_GL_SwapWindow(gAppState->window);
        transientStorage->freeMemory();

        reloadGameLib(transientStorage);
    }

    SDL_StopTextInput();
    close(gAppState, &gGlContext);

    return 0;
}
