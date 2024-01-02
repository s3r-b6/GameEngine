#include "engine_lib.h"
#include "initialization.h"

void update();
void render(RenderState *rs);

ProgramState pState;
RenderState rState;

struct Pos {};

void update() {}

void render(RenderState *rs) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(rs->programID);

    // Enable vertex position
    glEnableVertexAttribArray(rs->vertexPos2DLocation);

    // Set vertex data
    glBindBuffer(GL_ARRAY_BUFFER, rs->VBO);
    glVertexAttribPointer(rs->vertexPos2DLocation, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), NULL);

    // Set index data and render
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rs->IBO);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

    // Disable vertex position
    glDisableVertexAttribArray(rs->vertexPos2DLocation);
    glUseProgram(NULL);
}

int main(int argc, char *args[]) {
    ProgramState pState = {
        .running = true,
        .screen_width = 640,
        .screen_height = 640,
        .window = NULL,
        .context = NULL,

        .permanentStorage = new BumpAllocator(MB(10)),
        .transientStorage = new BumpAllocator(MB(10)),
    };

    size_t fsize;
    char *filecontents =
        pState.permanentStorage->readFile("./src/.clangd", &fsize);

    printf("%s", filecontents);

    if (!initSDLandGL(&pState, &rState)) {
        printf("ERROR: Failed to initialize SDL or OpenGL!\n");
        return -1;
    }

    SDL_Event event;
    SDL_StartTextInput();

    while (pState.running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                pState.running = false;
            } else if (event.type == SDL_TEXTINPUT) {
                int x = 0, y = 0;
                SDL_GetMouseState(&x, &y);

                char key = event.text.text[0];
                SDL_Log("Pressed %c", key);
            }
        }

        render(&rState);
        SDL_GL_SwapWindow(pState.window);
    }

    SDL_StopTextInput();
    close(&pState, &rState);
    return 0;
}
