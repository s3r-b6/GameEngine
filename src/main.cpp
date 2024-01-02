#include "engine_lib.h"
#include "initialization.h"

ProgramState g_pState = {0};
GLContext g_glContext = {0};

void render(GLContext *glContext, ProgramState *pState) {
    glClearColor(119.f / 255.f, 33.f / 255.f, 111.f / 255.f, 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, pState->width, pState->height);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main(int argc, char *args[]) {
    g_pState = {
        .running = true,
        .width = 1280,
        .height = 720,
        .permanentStorage = new BumpAllocator(MB(10)),
        .transientStorage = new BumpAllocator(MB(10)),
    };

    if (!initSDLandGL(&g_pState, &g_glContext, g_pState.transientStorage)) {
        SDL_Log("ERROR: Failed to initialize SDL or OpenGL!");
        return -1;
    }

    // Free the memory used for initialization
    g_pState.transientStorage->freeMemory();

    // Get initial window size
    int width, height;
    SDL_GL_GetDrawableSize(g_pState.sdl_window, &width, &height);
    g_pState.width = width;
    g_pState.height = height;

    SDL_Log("Initial screen size: %dx%d", g_pState.width, g_pState.height);

    SDL_Event event;
    SDL_StartTextInput();

    while (g_pState.running) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
            case SDL_QUIT: {
                g_pState.running = false;
                break;
            }

            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int width, height;
                    SDL_GL_GetDrawableSize(g_pState.sdl_window, &width,
                                           &height);
                    g_pState.width = width;
                    g_pState.height = height;
                }
                break;
            }

            case SDL_TEXTINPUT: {
                int x = 0, y = 0;
                SDL_GetMouseState(&x, &y);

                char key = event.text.text[0];
                SDL_Log("Pressed '%c'", key);
                break;
            }
            }
        }

        render(&g_glContext, &g_pState);
        SDL_GL_SwapWindow(g_pState.sdl_window);
    }

    SDL_StopTextInput();
    close(&g_pState, &g_glContext);
    return 0;
}
