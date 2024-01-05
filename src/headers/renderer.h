#pragma once

#include "engine_lib.h"

#include "assets.h"

struct Transform {
    glm::ivec2 atlasOffset;
    glm::ivec2 spriteSize;
    glm::vec2 pos;
    glm::vec2 size;

    u8 atlasIdx;
};

constexpr int MAX_TRANSFORMS = 1000;

struct RenderData {
    Transform transforms[MAX_TRANSFORMS] = {};
    int transformCount = 0;
};

constexpr GLsizei MAX_TEXTURES = 8;

// Struct that holds all data relevant to the renderer
// TODO: align this
struct GLContext {
    GLuint programID = 0;
    GLuint textureIDs[MAX_TEXTURES];
    GLuint transformSBOID;
    GLuint screenSizeID;

    u16 usedTextures = 0;
};

bool loadTextureAtlas(char const *texturePath, GLContext *glContext,
                      GLenum glTextureIdx);

void draw_sprite(RenderData *renderData, SpriteID spriteID, glm::ivec2 pos,
                 glm::ivec2 size);

global RenderData *gRenderData;
global GLContext gGlContext;
