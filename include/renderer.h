#pragma once

#include <glad/glad.h>

#include "glm/ext/matrix_float4x4.hpp"

#include "./assets.h"
#include "./globals.h"

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

struct OrthographicCamera {
    vec2 pos;
    vec2 dimensions;

    mat4x4 getProjectionMatrix(int width, int height);
    ivec2 getMousePosInWorld(ivec2 mousePos, ivec2 screenSize);
};

struct RenderData {
    Transform transforms[MAX_TRANSFORMS] = {};
    int transformCount = 0;

    float clearColor[3];

    OrthographicCamera gameCamera;
};

// Struct that holds all data relevant to the renderer
struct GLContext {
    GLuint programID = 0;
    GLuint textureIDs[MAX_TEXTURES];
    GLuint transformSBOID;
    GLuint screenSizeID;
    GLuint orthoProjectionID;

    u16 usedTextures = 0;
};

bool loadTextureAtlas(char const *texturePath, GLContext *glContext, GLenum glTextureIdx);
void draw_sprite(RenderData *renderData, SpriteID spriteID, glm::vec2 pos, glm::vec2 size);
void draw_tile(RenderData *renderData, u8 x, u8 y, u8 atlasIdx, glm::vec2 pos);
