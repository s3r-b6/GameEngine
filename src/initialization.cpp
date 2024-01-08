// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./headers/initialization.h"
#include "./headers/renderer.h"

// Append to the shaders location the file
#define SHADER_SRC(termination) "../assets/shaders/" termination

// TODO: Make logs different based on severity...

bool initImgui(ImguiState *imgui, ProgramState *pState) {
    IMGUI_CHECKVERSION();
    imgui->ctxt = ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplSDL2_InitForOpenGL(pState->window, pState->glContext);
    ImGui_ImplOpenGL3_Init();

    ImGui::GetAllocatorFunctions(&imgui->p_alloc_func, &imgui->p_free_func,
                                 &imgui->p_user_data);

    return true;
}

bool initSDLandGL(ProgramState *ps, GLContext *rs, RenderData *renderData,
                  BumpAllocator *transientStorage) {
    // Initialize SDL
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_Log("SDL could not initialize! SDL Error: %s\n",
                    SDL_GetError());
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);

        // Create window and context
        SDL_Window *window = SDL_CreateWindow(
            "Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            ps->width, ps->height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        if (!window) {
            SDL_Log("Window could not be created! SDL Error: %s\n",
                    SDL_GetError());
            return false;
        }

        SDL_GLContext context = SDL_GL_CreateContext(window);
        if (!context) {
            SDL_Log("OpenGL context could not be created! SDL Error: %s\n",
                    SDL_GetError());
            return false;
        }

        ps->glContext = context;
        ps->window = window;
    }

    // Initialize glew, set VSync, OpenGL
    {
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            SDL_Log("Error initializing glad!\n");
            return false;
        }

        // Use AdaptiveVsync (-1) Vsync (1) or do not (0)
        if (SDL_GL_SetSwapInterval(-1) < 0) {
            SDL_Log("Warning: Unable to set AdaptiveVsync! Trying to set VSync SDL Error: %s\n",
                    SDL_GetError());
        }

        if (SDL_GL_SetSwapInterval(1) < 0) {
            SDL_Log("Warning: Unable to set VSync! SDL Error: %s\n",
                    SDL_GetError());
            return false;
        }


        // Initialize OpenGL
        if (!initGL(rs, transientStorage, renderData)) {
            SDL_Log("Unable to initialize OpenGL!");
            return false;
        }
    }

    return true;
}

bool initGL(GLContext *glContext, BumpAllocator *tStorage,
            RenderData *renderData) {
    glContext->programID = glCreateProgram();

    size_t vertSourceSize = 0, fragSourceSize = 0;
    char *vertSource =
        plat_readFile(SHADER_SRC("vert.glsl"), &vertSourceSize, tStorage);
    char *fragSource =
        plat_readFile(SHADER_SRC("frag.glsl"), &fragSourceSize, tStorage);

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

    glContext->screenSizeID =
        glGetUniformLocation(glContext->programID, "screenSize");
    glContext->orthoProjectionID =
        glGetUniformLocation(glContext->programID, "orthoProjection");

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

void close(ProgramState *ps, GLContext *gc) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(gc->programID);
    SDL_DestroyWindow(ps->window);
    ps->window = NULL;
    SDL_Quit();
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

    char *infoLog =
        reinterpret_cast<char *>(transientStorage->alloc(maxLength));

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
    char *infoLog =
        reinterpret_cast<char *>(transientStorage->alloc(maxLength));

    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) { SDL_Log("%s", infoLog); }
}
