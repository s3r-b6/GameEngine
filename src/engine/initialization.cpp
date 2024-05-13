// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./initialization.h"
#include "./audio.h"
#include "./input.h"
#include "./tiles.h"

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alut.h"

#include "./shaders.h"

// Append to the shaders location the file
#define SHADER_SRC(termination) "../assets/shaders/" termination

bool initialize(BumpAllocator *permStorage, BumpAllocator *tempStorage) {
    appState = (ProgramState *)permStorage->alloc(sizeof(ProgramState));
    gameState = (GameState *)permStorage->alloc(sizeof(GameState));
    renderData = (RenderData *)permStorage->alloc(sizeof(RenderData));
    input = (Input *)permStorage->alloc(sizeof(Input));
    glContext = (GLContext *)permStorage->alloc(sizeof(GLContext));
    alState = (ALState *)permStorage->alloc(sizeof(ALState));

    if (!appState || !renderData || !gameState || !input) {
        engine_log("ERROR: Failed to alloc globalState");
        return false;
    }

    gameState->entityManager =
        new (permStorage->alloc(sizeof(EntityManager))) EntityManager(1024, permStorage);
    gameState->tileManager = (TileManager *)permStorage->alloc(sizeof(TileManager));

    if (!gameState->entityManager || !gameState->tileManager) {
        engine_log("ERROR: Failed to alloc gameState");
        return false;
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

    if (!initOpenAL()) {
        engine_log("ERROR: Failed to initialize OpenAL");
        return false;
    }

    if (!initSDLandGL(tempStorage)) {
        engine_log("ERROR: Failed to initialize SDL or OpenGL!");
        return false;
    }

    // Get initial window size. It should have been initialized to defaults,
    // but who knows
    SDL_GL_GetDrawableSize(appState->window, &appState->screenSize.x, &appState->screenSize.y);

    // This is a dumb hack; at least for my WM, the window is resizable unless
    // it has been in fullscreen before
    SDL_SetWindowResizable(appState->window, SDL_FALSE);
    SDL_SetWindowFullscreen(appState->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_SetWindowFullscreen(appState->window, SDL_FALSE);

    SDL_StartTextInput();

    engine_log("PermStorage use after init: %d/%d", (permStorage->used / MB(1)),
               (permStorage->size / MB(1)));

    return true;
}

inline bool initSDLandGL(BumpAllocator *tempStorage) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        engine_log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window and context
    SDL_Window *window = SDL_CreateWindow(
        "Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, appState->screenSize.x,
        appState->screenSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        engine_log("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        engine_log("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    appState->glContext = context;
    appState->window = window;

    // Initialize glew, set VSync, OpenGL
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        engine_log("Error initializing glad!\n");
        return false;
    }

    // Use AdaptiveVsync (-1) Vsync (1) or do not (0)
    if (SDL_GL_SetSwapInterval(-1) < 0) {
        engine_log("Warning: Unable to set AdaptiveVsync! Trying to set VSync "
                   "SDL Error: %s\n",
                   SDL_GetError());

        if (SDL_GL_SetSwapInterval(1) < 0) {
            engine_log("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
            return false;
        }
    }

    // Initialize OpenGL
    if (!initGL(tempStorage)) {
        engine_log("Unable to initialize OpenGL!");
        return false;
    }

    return true;
}

inline bool initGL(BumpAllocator *tempStorage) {
    glContext->programID = glCreateProgram();

    GLuint vertexShaderID, fragmentShaderID;
    if (!loadShaders(SHADER_SRC("vert.glsl"), SHADER_SRC("frag.glsl"), vertexShaderID,
                     fragmentShaderID, tempStorage))
        return false;

    glAttachShader(glContext->programID, vertexShaderID);
    glAttachShader(glContext->programID, fragmentShaderID);

    glLinkProgram(glContext->programID);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(glContext->programID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        engine_log("Error linking program %d!\n", glContext->programID);
        printProgramLog(glContext->programID, tempStorage);
        return false;
    }

    glDetachShader(glContext->programID, vertexShaderID);
    glDetachShader(glContext->programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

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
    engine_log("Closing OpenAL");
    exitOpenAL();

    engine_log("Closing SDL resources");
    SDL_StopTextInput();
    glDeleteProgram(glContext->programID);
    SDL_DestroyWindow(appState->window);
    appState->window = NULL;

    engine_log("Quitting");
    SDL_Quit();
}

void printProgramLog(uint program, BumpAllocator *tempStorage) {
    // Make sure name is shader
    if (!glIsProgram(program)) {
        engine_log("Name %d is not a program", program);
        return;
    }

    int infoLogLength = 0;
    int maxLength = 0;

    // Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    char *infoLog = reinterpret_cast<char *>(tempStorage->alloc(maxLength));

    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) engine_log("%s", infoLog);
}

// TODO: Fix this
// rn: mostly yanked from: https://github.com/ffainelli/openal-example/tree/master
bool initOpenAL() {
    alState->enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (alState->enumeration == AL_FALSE) fprintf(stderr, "enumeration extension not available\n");

    const ALCchar *defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    alState->device = alcOpenDevice(defaultDeviceName);

    if (!alState->device) {
        engine_log("unable to open default device\n");
        return false;
    }

    alState->context = alcCreateContext(alState->device, NULL);
    if (!alcMakeContextCurrent(alState->context)) {
        engine_log("failed to make default context\n");
        return false;
    }
    displayErrorsAL(__FILE__, __LINE__);

    engine_log("Device: %s", alcGetString(alState->device, ALC_DEVICE_SPECIFIER));
    displayErrorsAL(__FILE__, __LINE__);

    /* set orientation */
    alListener3f(AL_POSITION, 0, 0, 1.0f);
    displayErrorsAL(__FILE__, __LINE__);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    displayErrorsAL(__FILE__, __LINE__);
    alListenerfv(AL_ORIENTATION, alState->listenerOri);
    displayErrorsAL(__FILE__, __LINE__);

    ALuint *sources[2] = {&alState->source, &alState->bgSource};
    alGenSources((ALuint)2, sources[0]);
    displayErrorsAL(__FILE__, __LINE__);

    alSourcef(alState->source, AL_PITCH, 1);
    displayErrorsAL(__FILE__, __LINE__);
    alSourcef(alState->source, AL_GAIN, 1);
    displayErrorsAL(__FILE__, __LINE__);
    alSource3f(alState->source, AL_POSITION, 0, 0, 0);
    displayErrorsAL(__FILE__, __LINE__);
    alSource3f(alState->source, AL_VELOCITY, 0, 0, 0);
    displayErrorsAL(__FILE__, __LINE__);
    alSourcei(alState->source, AL_LOOPING, AL_FALSE);
    displayErrorsAL(__FILE__, __LINE__);

    ALuint *buffers[2] = {&alState->buffer, &alState->bgBuffer};
    alGenBuffers(2, buffers[0]);
    displayErrorsAL(__FILE__, __LINE__);

    return true;
}

void exitOpenAL() {
    alDeleteSources(1, &alState->source);
    alDeleteBuffers(1, &alState->buffer);
    alDeleteSources(1, &alState->bgSource);
    alDeleteBuffers(1, &alState->bgBuffer);
    alState->device = alcGetContextsDevice(alState->context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(alState->context);
    alcCloseDevice(alState->device);
}
