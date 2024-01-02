#include "engine_lib.h"
#include "initialization.h"

ProgramState pState;
GLContext renderState;

void render(GLContext *glContext) {
    glClearColor(119.f / 255.f, 33.f / 255.f, 111.f / 255.f, 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, pState.width, pState.height);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(int argc, char *args[]) {
    ProgramState programState = {
        .running = true,
        .width = 1280,
        .height = 720,
        .sdl_window = NULL,
        .sdl_context = NULL,

        .permanentStorage = new BumpAllocator(MB(10)),
        .transientStorage = new BumpAllocator(MB(10)),
    };

    if (!initSDLandGL(&programState, &renderState,
                      programState.transientStorage)) {
        printf("ERROR: Failed to initialize SDL or OpenGL!");
        return -1;
    }

    // Free the memory used for initialization
    programState.transientStorage->freeMemory();

    SDL_Event event;
    SDL_StartTextInput();

    while (programState.running) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
            case SDL_QUIT: {
                programState.running = false;
                break;
            }

            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    SDL_GetWindowSize(programState.sdl_window,
                                      &programState.width,
                                      &programState.height);

                    // SDL_Log("New screen size: (%d, %d)", programState.width,
                    // programState.height);
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

        render(&renderState);
        SDL_GL_SwapWindow(programState.sdl_window);
    }

    SDL_StopTextInput();
    close(&programState, &renderState);
    return 0;
}
