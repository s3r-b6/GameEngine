#include "./shaders.h"

#include "./engine_global.h"
#include "./platform.h"
#include "./renderer.h"

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

void printShaderLog(uint shader, BumpAllocator *tempStorage) {
    if (!glIsShader(shader)) {
        engine_log("Name %d is not a shader", shader);
        return;
    }

    int infoLogLength = 0;
    int maxLength = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = reinterpret_cast<char *>(tempStorage->alloc(maxLength));

    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) { engine_log("%s", infoLog); }
}

bool loadShaders(char *vertShaderPath, char *fragShaderPath, BumpAllocator *tempStorage) {
    size_t vertSourceSize = 0, fragSourceSize = 0;

    u64 vertTime = plat_getFileTimestamp(vertShaderPath);
    u64 fragTime = plat_getFileTimestamp(fragShaderPath);

    char *vertSource = plat_readFile(vertShaderPath, &vertSourceSize, tempStorage);
    char *fragSource = plat_readFile(fragShaderPath, &fragSourceSize, tempStorage);

    if (vertTime > fragTime) {
        glContext->shadersTimestamp = vertTime;
    } else {
        glContext->shadersTimestamp = fragTime;
    }

    if (!vertSource) { engine_log("Failed to read vertex shader sources"); }
    if (!fragSource) { engine_log("Failed to read fragment shader sources"); }
    if (!vertSource || !fragSource) return false;

    GLuint newProgramID = glCreateProgram();
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShaderID, 1, &vertSource, NULL);
    glShaderSource(fragmentShaderID, 1, &fragSource, NULL);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);

    GLint vShaderCompiled = GL_FALSE;
    GLint fShaderCompiled = GL_FALSE;

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &vShaderCompiled);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &fShaderCompiled);

    if (vShaderCompiled != GL_TRUE) {
        engine_log("Unable to compile vertex shader %d!\n", vertexShaderID);
        printShaderLog(vertexShaderID, tempStorage);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        glDeleteProgram(newProgramID);
        return false;
    }

    if (fShaderCompiled != GL_TRUE) {
        engine_log("Unable to compile fragment shader %d!\n", fragmentShaderID);
        printShaderLog(fragmentShaderID, tempStorage);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        glDeleteProgram(newProgramID);
        return false;
    }

    glAttachShader(newProgramID, vertexShaderID);
    glAttachShader(newProgramID, fragmentShaderID);

    glLinkProgram(newProgramID);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(newProgramID, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        engine_log("Error linking program %d!\n", newProgramID);
        printProgramLog(newProgramID, tempStorage);
        glDetachShader(newProgramID, vertexShaderID);
        glDetachShader(newProgramID, fragmentShaderID);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        glDeleteProgram(newProgramID);
        return false;
    }

    glDetachShader(newProgramID, vertexShaderID);
    glDetachShader(newProgramID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    glUseProgram(newProgramID);
    if (glContext->programID != 0) { glDeleteProgram(glContext->programID); }
    glContext->programID = newProgramID;

    return true;
}
