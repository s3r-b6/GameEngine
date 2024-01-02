#pragma once

#include "engine_lib.h"

struct ProgramState {
    bool running = true;
    int screen_width = 640;
    int screen_height = 640;
    SDL_Window *window = NULL;
    SDL_GLContext context;

    BumpAllocator *permanentStorage;
    BumpAllocator *transientStorage;
};

struct RenderState {
    GLuint programID = 0;
    GLint vertexPos2DLocation = -1;
    GLuint VBO = 0;
    GLuint IBO = 0;
};

bool initSDLandGL(ProgramState *ps, RenderState *rs);
bool initGL(RenderState *rs);
void close(ProgramState *ps, RenderState *rs);

void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

bool initSDLandGL(ProgramState *pProgramState, RenderState *pRenderState) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    pProgramState->window = SDL_CreateWindow(
        "Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        pProgramState->screen_width, pProgramState->screen_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (pProgramState->window == NULL) {
        SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Create context
    pProgramState->context = SDL_GL_CreateContext(pProgramState->window);
    if (pProgramState->context == NULL) {
        SDL_Log("OpenGL context could not be created! SDL Error: %s\n",
                SDL_GetError());
        return false;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        SDL_Log("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        return false;
    }

    // Use Vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
        SDL_Log("Warning: Unable to set VSync! SDL Error: %s\n",
                SDL_GetError());
        return false;
    }

    // Initialize OpenGL
    if (!initGL(pRenderState)) {
        SDL_Log("Unable to initialize OpenGL!\n");
        return false;
    }

    return true;
}

void printProgramLog(GLuint program) {
    // Make sure name is shader
    if (!glIsProgram(program)) {
        SDL_Log("Name %d is not a program\n", program);
        return;
    }

    // Program log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    // Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    // Allocate string
    char *infoLog = new char[maxLength];

    // Get info log
    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) SDL_Log("%s\n", infoLog);

    // Deallocate string
    delete[] infoLog;
}

void printShaderLog(GLuint shader) {
    // Make sure name is shader
    if (!glIsShader(shader)) {
        SDL_Log("Name %d is not a shader\n", shader);
        return;
    }

    // Shader log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    // Get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    // Allocate string
    char *infoLog = new char[maxLength];

    // Get info log
    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) { SDL_Log("%s\n", infoLog); }

    // Deallocate string
    delete[] infoLog;
}

bool initGL(RenderState *pRenderState) {
    pRenderState->programID = glCreateProgram();

    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Get vertex source
    const GLchar *vertexShaderSource[] = {
        "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( "
        "LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"};

    // Set vertex source
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

    // Compile vertex source
    glCompileShader(vertexShader);

    // Check vertex shader for errors
    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);

    if (vShaderCompiled != GL_TRUE) {
        SDL_Log("Unable to compile vertex shader %d!\n", vertexShader);
        printShaderLog(vertexShader);
        return false;
    }

    // Attach vertex shader to program
    glAttachShader(pRenderState->programID, vertexShader);

    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Get fragment source
    const GLchar *fragmentShaderSource[] = {
        "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( "
        "1.0, 1.0, 1.0, 1.0 ); }"};

    // Set fragment source
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

    // Compile fragment source
    glCompileShader(fragmentShader);

    // Check fragment shader for errors
    GLint fShaderCompiled = GL_FALSE;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
    if (fShaderCompiled != GL_TRUE) {
        SDL_Log("Unable to compile fragment shader %d!\n", fragmentShader);
        printShaderLog(fragmentShader);
        return false;
    }

    // Attach fragment shader to program
    glAttachShader(pRenderState->programID, fragmentShader);

    // Link program
    glLinkProgram(pRenderState->programID);

    // Check for errors
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(pRenderState->programID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        SDL_Log("Error linking program %d!\n", pRenderState->programID);
        printProgramLog(pRenderState->programID);
        return false;
    }

    // Get vertex attribute location
    pRenderState->vertexPos2DLocation =
        glGetAttribLocation(pRenderState->programID, "LVertexPos2D");
    if (pRenderState->vertexPos2DLocation == -1) {
        SDL_Log("LVertexPos2D is not a valid glsl program variable!\n");
        return false;
    }

    // Initialize clear color
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // VBO data
    GLfloat vertexData[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};
    // IBO data
    GLuint indexData[] = {0, 1, 2, 3};

    // Create VBO
    glGenBuffers(1, &pRenderState->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, pRenderState->VBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData,
                 GL_STATIC_DRAW);

    // Create IBO
    glGenBuffers(1, &pRenderState->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pRenderState->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData,
                 GL_STATIC_DRAW);

    return true;
}

void close(ProgramState *ps, RenderState *rs) {
    glDeleteProgram(rs->programID);
    SDL_DestroyWindow(ps->window);
    ps->window = NULL;
    SDL_Quit();
}
