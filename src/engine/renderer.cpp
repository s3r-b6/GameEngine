// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./renderer.h"
#include "./engine_global.h"
#include "./engine_lib.h"
#include "SDL2/SDL_log.h"
#include "tiles.h"
#include <vector>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#endif

// The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components:
//  1           grey                 2           grey, alpha
//  3           red, green, blue     4           red, green, blue, alpha
void parseTilemapData(u8 currentAtlas, u8 *data, int width, int height, int channels) {
    if (channels != 4) crash("oops");

    int x_tiles = width / TILESIZE, y_tiles = height / TILESIZE;
    for (int i = 0; i < x_tiles * y_tiles; i++) {
        TileBase t = TileBase{};
        t.x = u8(i % x_tiles);
        t.y = u8(i / x_tiles);
        t.atlasIdx = currentAtlas;

        tileManager->registerTile(t);
    }
}

bool loadTextureAtlas(char const *texturePath, GLenum glTextureIdx, bool createTiles) {
    local_persist u8 currentAtlas = 1;

    int height, width, channels;
    u8 *data = stbi_load(texturePath, &width, &height, &channels, 4);

    if (!data) {
        SDL_Log("Failed to load texture '%s'", texturePath);
        return false;
    }

    if (createTiles) {
        engine_log("Loading tilemap: '%s'", texturePath);
        parseTilemapData(currentAtlas, data, width, height, channels);
        engine_log("Total tiles: %d", tileManager->currentTiles);
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
    currentAtlas++;

    return true;
}

void render() {
    auto screenSize = appState->screenSize;
    glm::vec2 floatScreenSize = {static_cast<float>(screenSize.x),
                                 static_cast<float>(screenSize.y)};
    glUniform2fv(glContext->screenSizeID, 1, &floatScreenSize.x);

    glm::mat4x4 mat = renderData->gameCamera.getProjectionMatrix(screenSize.x, screenSize.y);
    glUniformMatrix4fv(glContext->orthoProjectionID, 1, GL_FALSE, &mat[0].x);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData->transformCount,
                    renderData->transforms);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->transformCount);
    renderData->transformCount = 0;
}

void UIrender() {
    auto screenSize = appState->screenSize;

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4x4 mat = renderData->uiCamera.getProjectionMatrix(screenSize.x, screenSize.y);
    glUniformMatrix4fv(glContext->orthoProjectionID, 1, GL_FALSE, &mat[0].x);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData->uiTransformCount,
                    renderData->uiTransforms);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->uiTransformCount);

    // 42 w/o help 193 with it
    // log("%d", renderData->uiTransformCount);
    renderData->uiTransformCount = 0;

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
