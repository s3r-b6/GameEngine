#include "./shaders.h"

#include "./engine_global.h"
#include "./platform.h"
#include "./renderer.h"

void printShaderLog(uint shader, BumpAllocator *tempStorage) {
    // Make sure name is shader
    if (!glIsShader(shader)) {
        engine_log("Name %d is not a shader", shader);
        return;
    }

    // Shader log length
    int infoLogLength = 0;
    int maxLength = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char *infoLog = reinterpret_cast<char *>(tempStorage->alloc(maxLength));

    glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
    if (infoLogLength > 0) { engine_log("%s", infoLog); }
}

bool loadShaders(char *vertShaderPath, char *fragShaderPath, GLuint &vertexShaderID,
                 GLuint &fragmentShaderID, BumpAllocator *tempStorage) {
    size_t vertSourceSize = 0, fragSourceSize = 0;
    char *vertSource = plat_readFile(vertShaderPath, &vertSourceSize, tempStorage);
    char *fragSource = plat_readFile(fragShaderPath, &fragSourceSize, tempStorage);

    u64 vertTime = plat_getFileTimestamp(vertShaderPath);
    u64 fragTime = plat_getFileTimestamp(fragShaderPath);

    if (vertTime < fragTime) {
        glContext->shadersTimestamp = vertTime;
    } else {
        glContext->shadersTimestamp = fragTime;
    }

    if (!vertSource) { engine_log("Failed to read vertex shader sources"); }
    if (!fragSource) { engine_log("Failed to read fragment shader sources"); }
    if (!vertSource || !fragSource) return false;

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

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
        engine_log("Unable to compile vertex shader %d!\n", vertexShaderID);
        printShaderLog(vertexShaderID, tempStorage);
    }

    if (fShaderCompiled != GL_TRUE) {
        engine_log("Unable to compile fragment shader %d!\n", fragmentShaderID);
        printShaderLog(fragmentShaderID, tempStorage);
    }

    if (fShaderCompiled != GL_TRUE || vShaderCompiled != GL_TRUE) return false;

    return true;
}
