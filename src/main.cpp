#include "engine_lib.h"
#include "initialization.h"

ProgramState g_appState = {0};
GLContext g_glContext = {0};

// I don't think inlining is strictly necessary, but this functions are only
// called here and I mainly extracted them to make this file more manageable.
//
// TODO: Test this for performance when I am actually rendering something
// complex
inline void render(GLContext *glContext, ProgramState *pState) {
    glClearColor(119.f / 255.f, 33.f / 255.f, 111.f / 255.f, 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, pState->width, pState->height);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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

    if (!initSDLandGL(&g_appState, &g_glContext, g_appState.transientStorage)) {
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

    while (g_appState.running) {
        while (SDL_PollEvent(&event) != 0) {
            handleSDLevents(&event);
        }

        render(&g_glContext, &g_appState);
        SDL_GL_SwapWindow(g_appState.window);
    }

    SDL_StopTextInput();
    close(&g_appState, &g_glContext);
    return 0;
}
