#pragma once

#include "./assets.h"
#include "./types.h"

#include <glad.h>

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

    int fontHeight;
    int transformCount = 0;
    int uiTransformCount = 0;

    float clearColor[3];

    OrthographicCamera gameCamera;
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

bool loadTextureAtlas(char const *texturePath, GLContext *glContext, GLenum glTextureIdx);
void draw_sprite(RenderData *renderData, SpriteID spriteID, glm::vec2 pos, glm::vec2 size);
void draw_tile(RenderData *renderData, u8 x, u8 y, u8 atlasIdx, glm::vec2 pos);

void render_ui(glm::mat4x4 proj, GlobalState *g);
void render(GlobalState *g);

void draw_ui_text_formatted(RenderData *renderData, vec2 pos, float fontSize, const char *text,
                            ...);
void draw_ui_text(RenderData *renderData, vec2 pos, float fontSize, const char *text);
