#include "engine_lib.h"

#include "initialization.h"

#include "renderer.h"

#include "game.h"

ProgramState g_appState = {0};
GLContext g_glContext = {0};
RenderData g_renderData = {};

// I don't think inlining is strictly necessary, but this functions are only
// called here and I mainly extracted them to make this file more manageable.
//
// TODO: Test this for performance when I am actually rendering something
// complex
inline void render() {
    glClearColor(119.f / 255.f, 33.f / 255.f, 111.f / 255.f, 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, g_appState.width, g_appState.height);

    glm::vec2 screenSize = {(float)g_appState.height, (float)g_appState.width};
    glUniform2fv(g_glContext.screenSizeID, 1, &screenSize.x);

    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                        sizeof(Transform) * g_renderData.transformCount,
                        g_renderData.transforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g_renderData.transformCount);

        g_renderData.transformCount = 0;
    }
}

inline void handleSDLevents(SDL_Event *event) {
    switch (event->type) {
    case SDL_QUIT: {
        g_appState.running = false;
        break;
    }

    case SDL_WINDOWEVENT: {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            SDL_GL_GetDrawableSize(g_appState.window, &g_appState.width,
                                   &g_appState.height);
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
    g_appState = {
        .running = true,
        .width = 1280,
        .height = 720,
        .permanentStorage = new BumpAllocator(MB(10)),
        .transientStorage = new BumpAllocator(MB(10)),
    };

    if (!initSDLandGL(&g_appState, &g_glContext, &g_renderData,
                      g_appState.transientStorage)) {
        SDL_Log("ERROR: Failed to initialize SDL or OpenGL!");
        return -1;
    }

    // Free the memory used for initialization
    g_appState.transientStorage->freeMemory();

    // Get initial window size. It should have been initialized to defaults, but
    // who knows
    SDL_GL_GetDrawableSize(g_appState.window, &g_appState.width,
                           &g_appState.height);

    SDL_Event event;
    SDL_StartTextInput();

    loadTextureAtlas("../assets/textures/zelda-like/character.png",
                     &g_glContext, GL_TEXTURE0);
    loadTextureAtlas("../assets/textures/zelda-like/objects.png", &g_glContext,
                     GL_TEXTURE1);

    while (g_appState.running) {
        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
        }

        update_game(&g_renderData);
        render();
        SDL_GL_SwapWindow(g_appState.window);
    }

    SDL_StopTextInput();
    close(&g_appState, &g_glContext);
    return 0;
}
