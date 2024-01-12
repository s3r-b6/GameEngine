#pragma once

#include "./assets.h"
#include <glad/glad.h>

using glm::ivec2;
using glm::mat4;
using glm::mat4x4;
using glm::vec2;

struct Transform {
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 pos;
    vec2 size;

    uint atlasIdx;
    uint padding[3];
};

constexpr int MAX_TRANSFORMS = 256;
struct OrthographicCamera {
    vec2 pos;
    vec2 dimensions;

    mat4x4 getProjectionMatrix(int width, int height) {
        float aspectRatio =
            static_cast<float>(width) / static_cast<float>(height);

        float left = pos.x - (dimensions.x * aspectRatio) / 2.0;
        float right = pos.x + (dimensions.x * aspectRatio) / 2.0;

        float top = pos.y - dimensions.y / 2.0;
        float bottom = pos.y + dimensions.y / 2.0;

        return glm::ortho(left, right, bottom, top);
    }
};

struct RenderData {
    Transform transforms[MAX_TRANSFORMS] = {};
    int transformCount = 0;

    float clearColor[3];

    OrthographicCamera gameCamera;
};

constexpr GLsizei MAX_TEXTURES = 8;

// Struct that holds all data relevant to the renderer
// TODO: align this
struct GLContext {
    GLuint programID = 0;
    GLuint textureIDs[MAX_TEXTURES];
    GLuint transformSBOID;
    GLuint screenSizeID;
    GLuint orthoProjectionID;

    u16 usedTextures = 0;
};

bool loadTextureAtlas(char const *texturePath, GLContext *glContext,
                      GLenum glTextureIdx);

void draw_sprite(RenderData *renderData, SpriteID spriteID, glm::vec2 pos,
                 glm::vec2 size);
