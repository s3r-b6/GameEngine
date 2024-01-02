#include "initialization.h"
#include "engine_lib.h"

// Append to the shaders location the file
#define SHADER_SRC(termination) "./assets/shaders/" termination

// TODO: Make logs different based on severity...

bool initSDLandGL(ProgramState *ps, RenderState *rs,
                  BumpAllocator *transientStorage) {
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
    ps->window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, ps->screen_width,
                                  ps->screen_height,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (ps->window == NULL) {
        SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Create context
    ps->context = SDL_GL_CreateContext(ps->window);
    if (ps->context == NULL) {
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
    if (!initGL(rs, transientStorage)) {
        SDL_Log("Unable to initialize OpenGL!");
        return false;
    }

    return true;
}

void printProgramLog(GLuint program, BumpAllocator *transientStorage) {
    // Make sure name is shader
    if (!glIsProgram(program)) {
        SDL_Log("Name %d is not a program", program);
        return;
    }

    int infoLogLength = 0;
    int maxLength = 0;

    // Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    char *infoLog = (char *)transientStorage->alloc(maxLength);
    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) SDL_Log("%s", infoLog);
}

void printShaderLog(GLuint shader, BumpAllocator *transientStorage) {
    // Make sure name is shader
    if (!glIsShader(shader)) {
        SDL_Log("Name %d is not a shader", shader);
        return;
    }

    // Shader log length
    int infoLogLength = 0;
    int maxLength = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = (char *)transientStorage->alloc(maxLength);

    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) { SDL_Log("%s", infoLog); }
}

bool initGL(RenderState *rs, BumpAllocator *transientStorage) {
    rs->programID = glCreateProgram();

    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    size_t fsize;
    char *vertexShaderSource =
        transientStorage->readFile(SHADER_SRC("vert.hlsl"), &fsize);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader for errors
    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);

    if (vShaderCompiled != GL_TRUE) {
        SDL_Log("Unable to compile vertex shader %d!\n", vertexShader);
        printShaderLog(vertexShader, transientStorage);
        return false;
    }

    // Attach vertex shader to program
    glAttachShader(rs->programID, vertexShader);

    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    fsize = 0;
    GLchar *fragmentShaderSource =
        (GLchar *)transientStorage->readFile(SHADER_SRC("frag.hlsl"), &fsize);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLint fShaderCompiled = GL_FALSE;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
    if (fShaderCompiled != GL_TRUE) {
        SDL_Log("Unable to compile fragment shader %d!\n", fragmentShader);
        printShaderLog(fragmentShader, transientStorage);
        return false;
    }

    // Attach fragment shader to program
    glAttachShader(rs->programID, fragmentShader);

    // Link program
    glLinkProgram(rs->programID);

    // Check for errors
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(rs->programID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        SDL_Log("Error linking program %d!\n", rs->programID);
        printProgramLog(rs->programID, transientStorage);
        return false;
    }

    // Get vertex attribute location
    rs->vertexPos2DLocation =
        glGetAttribLocation(rs->programID, "LVertexPos2D");
    if (rs->vertexPos2DLocation == -1) {
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
    glGenBuffers(1, &rs->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, rs->VBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData,
                 GL_STATIC_DRAW);

    // Create IBO
    glGenBuffers(1, &rs->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rs->IBO);
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
