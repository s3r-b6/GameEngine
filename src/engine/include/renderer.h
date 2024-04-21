#pragma once

#include "./types.h"

#include <glad.h>

using glm::ivec2;
using glm::mat4;
using glm::mat4x4;
using glm::vec2;

struct Transform {
    ivec2 atlasOffset, spriteSize;
    vec2 pos, size;

    uint atlasIdx;
    uint padding[3];
};

struct OrthographicCamera {
    vec2 pos;
    vec2 dimensions;

    mat4x4 getProjectionMatrix(int width, int height) {
        float left = pos.x - dimensions.x / 2.0;
        float right = pos.x + dimensions.x / 2.0;
        float top = pos.y - dimensions.y / 2.0;
        float bottom = pos.y + dimensions.y / 2.0;

        return glm::ortho(left, right, bottom, top);
    }

    ivec2 getMousePosInWorld(ivec2 mousePos, ivec2 screenSize) {
        int xPos = (float)mousePos.x / (float)screenSize.x * dimensions.x;
        xPos += -dimensions.x / 2.0f + pos.x;

        int yPos = (float)mousePos.y / (float)screenSize.y * dimensions.y;
        yPos += -dimensions.y / 2.0f + pos.y;

        return ivec2(xPos, yPos) / TILESIZE;
    }
};

struct Glyph {
    glm::vec2 offset;
    glm::vec2 advance;
    glm::ivec2 textureCoords;
    glm::ivec2 size;
};

struct RenderData {
    Transform transforms[MAX_TRANSFORMS] = {};
    Transform uiTransforms[MAX_TRANSFORMS] = {};

    Glyph glyphs[127];

    int fontHeight = 0;
    int transformCount = 0;
    int uiTransformCount = 0;

    glm::vec3 clearColor;

    OrthographicCamera gameCamera;
    OrthographicCamera uiCamera;
};

// Struct that holds all data relevant to the renderer
struct GLContext {
    GLuint programID;
    GLuint fontAtlasID;
    GLuint textureIDs[MAX_TEXTURES];
    GLuint transformSBOID;
    GLuint screenSizeID;
    GLuint orthoProjectionID;

    u16 usedTextures = 0;
};

bool loadTextureAtlas(char const *texturePath, GLenum glTextureIdx, bool createTiles);
void render();
void ui_render();
