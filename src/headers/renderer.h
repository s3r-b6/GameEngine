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
        float left = pos.x - dimensions.x / 2.;
        float right = pos.x + dimensions.x / 2.;

        float top = pos.y - dimensions.y / 2.;
        float bottom = pos.y + dimensions.y / 2.;

        float near = 0.;
        float far = 1.;

        // clang-format off
        mat4x4 mat = mat4(
            2/(right-left), 0,              0,             -(right+left)/(right-left),
            0,              2/(top-bottom), 0,             -(top+bottom)/(top-bottom),
            0,              0,              -2/(far-near), -(far+near)/(far-near),
            0,              0,              0,             1);
        // clang-format on

        return mat;
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

void draw_sprite(RenderData *renderData, SpriteID spriteID, glm::ivec2 pos,
                 glm::ivec2 size);

global RenderData *gRenderData;
global GLContext gGlContext;
