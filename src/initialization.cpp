#include "initialization.h"
#include "engine_lib.h"

// Append to the shaders location the file
#define SHADER_SRC(termination) "../assets/shaders/" termination

// TODO: Make logs different based on severity...

bool initSDLandGL(ProgramState *ps, GLContext *rs,
                  BumpAllocator *transientStorage) {

    { // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_Log("SDL could not initialize! SDL Error: %s\n",
                    SDL_GetError());
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);

        // Create window
        ps->sdl_window = SDL_CreateWindow(
            "Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            ps->width, ps->height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        if (ps->sdl_window == NULL) {
            SDL_Log("Window could not be created! SDL Error: %s\n",
                    SDL_GetError());
            return false;
        }

        // Create context
        ps->sdl_context = SDL_GL_CreateContext(ps->sdl_window);
        if (ps->sdl_context == NULL) {
            SDL_Log("OpenGL context could not be created! SDL Error: %s\n",
                    SDL_GetError());
            return false;
        }
    }

    { // Initialize glew, set VSync, OpenGL
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            SDL_Log("Error initializing GLEW! %s\n",
                    glewGetErrorString(glewError));
            return false;
        }

        // Use Vsync
        // if (SDL_GL_SetSwapInterval(1) < 0) {
        //    SDL_Log("Warning: Unable to set VSync! SDL Error: %s\n",
        //            SDL_GetError());
        //    return false;
        //}

        // Initialize OpenGL
        if (!initGL(rs, transientStorage)) {
            SDL_Log("Unable to initialize OpenGL!");
            return false;
        }
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

bool initGL(GLContext *glContext, BumpAllocator *tStorage) {
    glContext->programID = glCreateProgram();

    size_t vertSourceSize = 0, fragSourceSize = 0;
    char *vertSource =
        tStorage->readFile(SHADER_SRC("vert.hlsl"), &vertSourceSize);
    char *fragSource =
        tStorage->readFile(SHADER_SRC("frag.hlsl"), &fragSourceSize);

    if (!vertSource) { SDL_Log("Failed to read vertex shader sources"); }
    if (!fragSource) { SDL_Log("Failed to read fragment shader sources"); }
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
        SDL_Log("Unable to compile vertex shader %d!\n", vertexShaderID);
        printShaderLog(vertexShaderID, tStorage);
    }

    if (fShaderCompiled != GL_TRUE) {
        SDL_Log("Unable to compile fragment shader %d!\n", fragmentShaderID);
        printShaderLog(fragmentShaderID, tStorage);
    }

    if (fShaderCompiled != GL_TRUE || vShaderCompiled != GL_TRUE) return false;

    glAttachShader(glContext->programID, vertexShaderID);
    glAttachShader(glContext->programID, fragmentShaderID);

    glLinkProgram(glContext->programID);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(glContext->programID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        SDL_Log("Error linking program %d!\n", glContext->programID);
        printProgramLog(glContext->programID, tStorage);
        return false;
    }

    glDetachShader(glContext->programID, vertexShaderID);
    glDetachShader(glContext->programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // This seems necessary
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext->programID);

    return true;
}

void close(ProgramState *ps, GLContext *gc) {
    glDeleteProgram(gc->programID);
    SDL_DestroyWindow(ps->sdl_window);
    ps->sdl_window = NULL;
    SDL_Quit();
}
