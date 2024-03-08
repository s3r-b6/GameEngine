// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./initialization.h"
#include "./audio.h"
#include "./engine_lib.h"
#include "./entities.h"
#include "./game.h"
#include "./globals.h"
#include "./input.h"
#include "./renderer.h"

#include <al.h>
#include <alc.h>
#include <alut.h>

// Append to the shaders location the file
#define SHADER_SRC(termination) "../assets/shaders/" termination

GlobalState *initialize(BumpAllocator *permStorage, BumpAllocator *tempStorage) {
    // GlobalState just stitches together all pointers
    GlobalState *g = (GlobalState *)permStorage->alloc(sizeof(GlobalState));

    {
        g->appState = (ProgramState *)permStorage->alloc(sizeof(ProgramState));
        g->gameState = (GameState *)permStorage->alloc(sizeof(GameState));
        g->renderData = (RenderData *)permStorage->alloc(sizeof(RenderData));
        g->input = (Input *)permStorage->alloc(sizeof(Input));
        g->glContext = (GLContext *)permStorage->alloc(sizeof(GLContext));
        g->alState = (ALState *)permStorage->alloc(sizeof(ALState));

        if (!g->appState || !g->renderData || !g->gameState || !g->input) {
            log(__FILE__, __LINE__, "ERROR: Failed to alloc globalState");
            return nullptr;
        }

        g->gameState->entityManager = (EntityManager *)permStorage->alloc(sizeof(EntityManager));
        g->gameState->tileManager = (TileManager *)permStorage->alloc(sizeof(TileManager));

        if (!g->gameState->entityManager || !g->gameState->tileManager) {
            log(__FILE__, __LINE__, "ERROR: Failed to alloc gameState");
            return nullptr;
        }

        g->gameState->initialized = false;

        g->input->mouseInWindow = true;
        g->input->showCursor = true;

        int numKeys;
        g->input->keyboardState = SDL_GetKeyboardState(&numKeys);
        log(__FILE__, __LINE__, "numKeys: %d", numKeys);

        g->appState->running = true;
        g->appState->screenSize = {1280, 720};
        g->appState->window = NULL;
        g->appState->glContext = NULL;

        g->renderData->clearColor[0] = 119.f / 255.f;
        g->renderData->clearColor[1] = 33.f / 255.f;
        g->renderData->clearColor[2] = 111.f / 255.f;
    }

    if (!initSDLandGL(tempStorage, g->appState, g->glContext, g->renderData)) {
        log(__FILE__, __LINE__, "ERROR: Failed to initialize SDL or OpenGL!");
        return nullptr;
    }

    // Get initial window size. It should have been initialized to defaults,
    // but who knows
    SDL_GL_GetDrawableSize(g->appState->window, &g->appState->screenSize.x,
                           &g->appState->screenSize.y);

    // This is a dumb hack; at least for my WM, the window is resizable unless
    // it has been in fullscreen before
    SDL_SetWindowResizable(g->appState->window, SDL_FALSE);
    SDL_SetWindowFullscreen(g->appState->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_SetWindowFullscreen(g->appState->window, SDL_FALSE);

    if (!initOpenAL(g->alState)) {
        log(__FILE__, __LINE__, "ERROR: Failed to initialize OpenAL");
        return nullptr;
    }

    SDL_StartTextInput();

    return g;
}

inline bool initSDLandGL(BumpAllocator *tempStorage, ProgramState *appState, GLContext *glContext,
                         RenderData *renderData) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        log(__FILE__, __LINE__, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
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
        log(__FILE__, __LINE__, "Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        log(__FILE__, __LINE__, "OpenGL context could not be created! SDL Error: %s\n",
            SDL_GetError());
        return false;
    }

    appState->glContext = context;
    appState->window = window;

    // Initialize glew, set VSync, OpenGL
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        log(__FILE__, __LINE__, "Error initializing glad!\n");
        return false;
    }

    // Use AdaptiveVsync (-1) Vsync (1) or do not (0)
    if (SDL_GL_SetSwapInterval(-1) < 0) {
        log(__FILE__, __LINE__,
            "Warning: Unable to set AdaptiveVsync! Trying to set VSync "
            "SDL Error: %s\n",
            SDL_GetError());

        if (SDL_GL_SetSwapInterval(1) < 0) {
            log(__FILE__, __LINE__, "Warning: Unable to set VSync! SDL Error: %s\n",
                SDL_GetError());
            return false;
        }
    }

    // Initialize OpenGL
    if (!initGL(tempStorage, glContext, renderData)) {
        log(__FILE__, __LINE__, "Unable to initialize OpenGL!");
        return false;
    }

    return true;
}

inline bool initGL(BumpAllocator *tempStorage, GLContext *glContext, RenderData *renderData) {
    glContext->programID = glCreateProgram();

    size_t vertSourceSize = 0, fragSourceSize = 0;
    char *vertSource = plat_readFile(SHADER_SRC("vert.glsl"), &vertSourceSize, tempStorage);
    char *fragSource = plat_readFile(SHADER_SRC("frag.glsl"), &fragSourceSize, tempStorage);

    if (!vertSource) { log(__FILE__, __LINE__, "Failed to read vertex shader sources"); }
    if (!fragSource) { log(__FILE__, __LINE__, "Failed to read fragment shader sources"); }
    if (!vertSource || !fragSource) return false;

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShaderID, 1, &vertSource, NULL);
    glShaderSource(fragmentShaderID, 1, &fragSource, NULL);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);

    // Check shader compilation for errors
    GLint vShaderCompiled = GL_FALSE;
    GLint fShaderCompiled = GL_FALSE;

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &vShaderCompiled);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &fShaderCompiled);

    if (vShaderCompiled != GL_TRUE) {
        log(__FILE__, __LINE__, "Unable to compile vertex shader %d!\n", vertexShaderID);
        printShaderLog(vertexShaderID, tempStorage);
    }

    if (fShaderCompiled != GL_TRUE) {
        log(__FILE__, __LINE__, "Unable to compile fragment shader %d!\n", fragmentShaderID);
        printShaderLog(fragmentShaderID, tempStorage);
    }

    if (fShaderCompiled != GL_TRUE || vShaderCompiled != GL_TRUE) return false;

    glAttachShader(glContext->programID, vertexShaderID);
    glAttachShader(glContext->programID, fragmentShaderID);

    glLinkProgram(glContext->programID);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(glContext->programID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        log(__FILE__, __LINE__, "Error linking program %d!\n", glContext->programID);
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

void close(GLContext *glContext, ProgramState *appState, ALState *alState) {
    log(__FILE__, __LINE__, "Closing OpenAL");
    exitOpenAL(alState);

    log(__FILE__, __LINE__, "Closing SDL resources");
    SDL_StopTextInput();
    glDeleteProgram(glContext->programID);
    SDL_DestroyWindow(appState->window);
    appState->window = NULL;

    log(__FILE__, __LINE__, "Quitting");
    SDL_Quit();
}

void printProgramLog(uint program, BumpAllocator *tempStorage) {
    // Make sure name is shader
    if (!glIsProgram(program)) {
        log(__FILE__, __LINE__, "Name %d is not a program", program);
        return;
    }

    int infoLogLength = 0;
    int maxLength = 0;

    // Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    char *infoLog = reinterpret_cast<char *>(tempStorage->alloc(maxLength));

    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) log(__FILE__, __LINE__, "%s", infoLog);
}

void printShaderLog(uint shader, BumpAllocator *tempStorage) {
    // Make sure name is shader
    if (!glIsShader(shader)) {
        log(__FILE__, __LINE__, "Name %d is not a shader", shader);
        return;
    }

    // Shader log length
    int infoLogLength = 0;
    int maxLength = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = reinterpret_cast<char *>(tempStorage->alloc(maxLength));

    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) { log(__FILE__, __LINE__, "%s", infoLog); }
}

// TODO: Fix this
// rn: mostly yanked from: https://github.com/ffainelli/openal-example/tree/master
bool displayErrorsAL(const std::string &filename, const std::uint_fast32_t line) {
    ALenum error = alGetError();

    if (error == AL_NO_ERROR) return true;

    switch (error) {
    case AL_INVALID_NAME:
        _log(filename, line, "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
        break;
    case AL_INVALID_ENUM:
        _log(filename, line,
             "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
        break;
    case AL_INVALID_VALUE:
        _log(filename, line, "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
        break;
    case AL_INVALID_OPERATION:
        _log(filename, line, "AL_INVALID_OPERATION: the requested operation is not valid");
        break;
    case AL_OUT_OF_MEMORY:
        _log(filename, line,
             "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of "
             "memory");
        break;
    default:
        _log(filename, line, "UNKNOWN AL ERROR: ", error);
    }
    return false;
}

bool initOpenAL(ALState *alState) {
    WavFile wavFile = {};

    alState->enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (alState->enumeration == AL_FALSE) fprintf(stderr, "enumeration extension not available\n");

    const ALCchar *defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    alState->device = alcOpenDevice(defaultDeviceName);

    if (!alState->device) {
        log(__FILE__, __LINE__, "unable to open default device\n");
        return false;
    }

    alState->context = alcCreateContext(alState->device, NULL);
    if (!alcMakeContextCurrent(alState->context)) {
        log(__FILE__, __LINE__, "failed to make default context\n");
        return false;
    }
    displayErrorsAL(__FILE__, __LINE__);

    log(__FILE__, __LINE__, "Device: %s", alcGetString(alState->device, ALC_DEVICE_SPECIFIER));
    displayErrorsAL(__FILE__, __LINE__);

    /* set orientation */
    alListener3f(AL_POSITION, 0, 0, 1.0f);
    displayErrorsAL(__FILE__, __LINE__);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    displayErrorsAL(__FILE__, __LINE__);
    alListenerfv(AL_ORIENTATION, alState->listenerOri);
    displayErrorsAL(__FILE__, __LINE__);

    alGenSources((ALuint)1, &alState->source);
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

    alGenBuffers(1, &alState->buffer);
    displayErrorsAL(__FILE__, __LINE__);

    signed char filename[64];
    const char *originalFilename = "../assets/audio/test.wav";

    strncpy((char *)filename, originalFilename, 64);
    filename[64 - 1] = '\0';

    alutLoadWAVFile(filename, &wavFile.format, &wavFile.data, &wavFile.size, &wavFile.freq,
                    &wavFile.loop);
    displayErrorsAL(__FILE__, __LINE__);

    alBufferData(alState->buffer, wavFile.format, wavFile.data, wavFile.size, wavFile.freq);
    displayErrorsAL(__FILE__, __LINE__);

    alSourcei(alState->source, AL_BUFFER, alState->buffer);
    displayErrorsAL(__FILE__, __LINE__);

    alSourcePlay(alState->source);
    displayErrorsAL(__FILE__, __LINE__);

    alGetSourcei(alState->source, AL_SOURCE_STATE, &alState->source_state);
    displayErrorsAL(__FILE__, __LINE__);
    while (alState->source_state == AL_PLAYING) {
        alGetSourcei(alState->source, AL_SOURCE_STATE, &alState->source_state);
        displayErrorsAL(__FILE__, __LINE__);
    }

    return true;
}

void exitOpenAL(ALState *alState) {
    alDeleteSources(1, &alState->source);
    alDeleteBuffers(1, &alState->buffer);
    alState->device = alcGetContextsDevice(alState->context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(alState->context);
    alcCloseDevice(alState->device);
}
