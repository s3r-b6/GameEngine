#include "renderer.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

bool loadTextureAtlas(char const *texturePath, GLContext *glContext,
                      GLenum glTextureIdx) {
    int height, width, channels;
    u8 *data = stbi_load(texturePath, &width, &height, &channels, 4);

    if (!data) {
        SDL_Log("Failed to load texture %s", texturePath);
        return false;
    }

    glActiveTexture(glTextureIdx);
    glBindTexture(GL_TEXTURE_2D,
                  glContext->textureIDs[glContext->usedTextures]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    glContext->usedTextures++;

    return true;
}

void draw_sprite(RenderData *renderData, SpriteID spriteID, glm::ivec2 pos,
                 glm::ivec2 size) {

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
