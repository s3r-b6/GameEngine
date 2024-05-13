// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./initialization.h"

#include "./input.h"
#include "./shaders.h"
#include "./tiles.h"

void initialize(BumpAllocator *permStorage, BumpAllocator *tempStorage) {
    appState = (ProgramState *)permStorage->alloc(sizeof(ProgramState));
    gameState = (GameState *)permStorage->alloc(sizeof(GameState));
    renderData = (RenderData *)permStorage->alloc(sizeof(RenderData));
    input = (Input *)permStorage->alloc(sizeof(Input));
    glContext = (GLContext *)permStorage->alloc(sizeof(GLContext));

    if (!appState || !renderData || !gameState || !input) {
        crash("ERROR: Failed to alloc globalState");
    }

    gameState->entityManager =
        new (permStorage->alloc(sizeof(EntityManager))) EntityManager(1024, permStorage);
    gameState->tileManager = (TileManager *)permStorage->alloc(sizeof(TileManager));

    if (!gameState->entityManager || !gameState->tileManager) {
        crash("ERROR: Failed to alloc gameState");
    }

    tileManager = gameState->tileManager;
    entityManager = gameState->entityManager;

    gameState->tileManager->tilemap = std::map<TileID, TileBase>();
    gameState->tileManager->animatedTiles = std::map<TileID, AnimatedTile>();
    gameState->tileManager->currentTiles = 0;
    gameState->initialized = false;

    input->mouseInWindow = true;
    input->showCursor = true;

    int numKeys;
    input->keyboardState = SDL_GetKeyboardState(&numKeys);

    engine_log("The keyboard has %d keys", numKeys);

    // I am not happy with this, but wasting 512 bytes seems not that bad
    input->previousKeyboardState = (u8 *)malloc(sizeof(u8) * numKeys);
    if (input->previousKeyboardState == NULL) {
        crash("Failed to alloc memory for the previous keyboard state");
    }

    memset((void *)input->previousKeyboardState, false, sizeof(u8) * numKeys);

    appState->running = true;
    appState->screenSize = {1280, 720};
    appState->window = NULL;
    appState->glContext = NULL;

    renderData->clearColor = {119.f / 255.f, 33.f / 255.f, 111.f / 255.f};

    if (!initSDLandGL(tempStorage)) { engine_log("ERROR: Failed to initialize SDL or OpenGL!"); }

    // Get initial window size. It should have been initialized to defaults,
    // but who knows
    SDL_GL_GetDrawableSize(appState->window, &appState->screenSize.x, &appState->screenSize.y);

    // This is a dumb hack; at least for my WM, the window is resizable unless
    // it has been in fullscreen before
    SDL_SetWindowResizable(appState->window, SDL_FALSE);
    SDL_SetWindowFullscreen(appState->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_SetWindowFullscreen(appState->window, SDL_FALSE);

    SDL_StartTextInput();

    engine_log("PermStorage use after init: %d/%lu", (permStorage->used / MB(1)),
               (permStorage->size / MB(1)));
}

inline bool initSDLandGL(BumpAllocator *tempStorage) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        engine_log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window and context
    SDL_Window *window = SDL_CreateWindow(
        "Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, appState->screenSize.x,
        appState->screenSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) { crash("Window could not be created! SDL Error: %s\n", SDL_GetError()); }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) { crash("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError()); }

    appState->glContext = context;
    appState->window = window;

    // Initialize glew, set VSync, OpenGL
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        crash("ERROR: unable to initialize glad!\n");
    }

    // Use AdaptiveVsync (-1) Vsync (1) or do not (0)
    if (SDL_GL_SetSwapInterval(-1) < 0) {
        engine_log("Warning: Unable to set AdaptiveVsync! Trying to set VSync "
                   "SDL Error: %s\n",
                   SDL_GetError());

        if (SDL_GL_SetSwapInterval(1) < 0) {
            engine_log("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
        }
    }

    // Initialize OpenGL
    if (!initGL(tempStorage)) { crash("ERROR: to initialize OpenGL!"); }

    if (!initAudio()) { crash("ERROR: Failed to initialize audio"); }

    return true;
}

inline bool initGL(BumpAllocator *tempStorage) {
    glContext->programID = glCreateProgram();

    if (!loadShaders(SHADER_SRC("vert.glsl"), SHADER_SRC("frag.glsl"), tempStorage))

        glContext->screenSizeID = glGetUniformLocation(glContext->programID, "screenSize");
    glContext->orthoProjectionID = glGetUniformLocation(glContext->programID, "orthoProjection");

    // This seems necessary
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // This is for the textureAtlases
    glGenTextures(MAX_TEXTURES, glContext->textureIDs);

    // This creates a buffer for the transforms
    glGenBuffers(1, &glContext->transformSBOID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext->transformSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS,
                 renderData->transforms, GL_DYNAMIC_DRAW);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext->programID);

    return true;
}

void close() {
    engine_log("Closing SDL resources");
    SDL_StopTextInput();
    glDeleteProgram(glContext->programID);
    SDL_DestroyWindow(appState->window);
    appState->window = NULL;

    engine_log("Quitting");
    SDL_Quit();
}

// TODO:
bool initAudio() { return true; }
