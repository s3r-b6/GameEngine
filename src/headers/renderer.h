#pragma once

#include "engine_lib.h"

#include "assets.h"

using glm::vec2, glm::ivec2, glm::mat4x4, glm::mat4;

struct Transform {
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 pos;
    vec2 size;

    GLuint atlasIdx;

    GLuint padding[3];
};

constexpr int MAX_TRANSFORMS = 256;
struct OrthographicCamera {
    vec2 pos;
    vec2 dimensions;

    mat4x4 getProjectionMatrix() {
        float left = pos.x - dimensions.x / 2.0;
        float right = pos.x + dimensions.x / 2.0;

        float top = pos.y - dimensions.y / 2.0;
        float bottom = pos.y + dimensions.y / 2.0;

        return glm::ortho(left, right, bottom, top);
    };
};

struct RenderData {
    Transform transforms[MAX_TRANSFORMS] = {};
    int transformCount = 0;

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

global RenderData *gRenderData;
global GLContext gGlContext;
