// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./renderer.h"
#include "SDL2/SDL_log.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h"
#endif

// TODO: This should use a map or something similar, so one texture can be freed
// and then I can ask which slot is free and occupy it again and so on
bool loadTextureAtlas(char const *texturePath, GLContext *glContext, GLenum glTextureIdx) {
    int height, width, channels;
    u8 *data = stbi_load(texturePath, &width, &height, &channels, 4);

    if (!data) {
        SDL_Log("Failed to load texture %s", texturePath);
        return false;
    }

    glActiveTexture(glTextureIdx);
    glBindTexture(GL_TEXTURE_2D, glContext->textureIDs[glContext->usedTextures]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 data);

    stbi_image_free(data);

    glContext->usedTextures++;

    return true;
}

void draw_sprite(RenderData *renderData, SpriteID spriteID, glm::vec2 pos, glm::vec2 size) {
    Sprite sp = get_sprite(spriteID);

    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = sp.spriteSize,
        .pos = pos,
        .size = size,

        .atlasIdx = sp.atlasIdx,
    };

    renderData->transforms[renderData->transformCount++] = t;
}

void draw_tile(RenderData *renderData, u8 x, u8 y, u8 atlasIdx, glm::vec2 pos) {
    Sprite sp = get_tile(x, y, atlasIdx);

    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = sp.spriteSize,
        .pos = pos,
        .size = {16, 16},

        .atlasIdx = sp.atlasIdx,
    };

    renderData->transforms[renderData->transformCount++] = t;
}

mat4x4 OrthographicCamera::getProjectionMatrix(int width, int height) {
    float left = pos.x - dimensions.x / 2.0;
    float right = pos.x + dimensions.x / 2.0;
    float top = pos.y - dimensions.y / 2.0;
    float bottom = pos.y + dimensions.y / 2.0;

    return glm::ortho(left, right, bottom, top);
}

ivec2 OrthographicCamera::getMousePosInWorld(ivec2 mousePos, ivec2 screenSize) {
    int xPos = (float)mousePos.x / (float)screenSize.x * dimensions.x;
    xPos += -dimensions.x / 2.0f + pos.x;

    int yPos = (float)mousePos.y / (float)screenSize.y * dimensions.y;
    yPos += -dimensions.y / 2.0f + pos.y;

    return ivec2(xPos, yPos) / TILESIZE;
}
