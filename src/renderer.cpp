// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./renderer.h"
#include "./engine_lib.h"
#include "./globals.h"
#include "SDL2/SDL_log.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#endif

// The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components:
//  1           grey                 2           grey, alpha
//  3           red, green, blue     4           red, green, blue, alpha
void parse_png_data(u8 *data, int width, int height, int channels) {
    local_persist u32 total_tiles = 0;
    if (channels != 4) crash("oops");
    // log("W%d H%d channels: %d", width, height, channels);

    int x_tiles = width / 16, y_tiles = height / 16;
    for (int i = 0; i < x_tiles * y_tiles; i++) {
        bool is_empty = true;
        for (int j = 0; j < 16 * 16; j++) {
            int y = i / x_tiles * 16 + j / 16;
            int x = i % x_tiles * 16 + j % 16;

            if (x >= width || y >= height) continue;

            u32 pixel_index = (y * width + x) * channels;
            u8 r = data[pixel_index], g = data[pixel_index + 1];
            u8 b = data[pixel_index + 2], a = data[pixel_index + 3];

            if (a != 0) {
                is_empty = false;
                break;
            }
        }

        if (!is_empty) {
            // printf("tile { %d, %d } is non-empty\n", i % x_tiles, i / x_tiles);
            total_tiles++;
        }
    }

    log("total tiles after loading file: %d", total_tiles);
}

// TODO: This should use a map or something similar, so one texture can be freed
// and then I can ask which slot is free and occupy it again and so on
bool loadTextureAtlas(char const *texturePath, GLContext *glContext, GLenum glTextureIdx, bool createTiles) {
    int height, width, channels;
    u8 *data = stbi_load(texturePath, &width, &height, &channels, 4);

    parse_png_data(data, width, height, channels);

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

void render(GlobalState *g) {
    auto screenSize = g->appState->screenSize;
    glm::vec2 floatScreenSize = {static_cast<float>(screenSize.x),
                                 static_cast<float>(screenSize.y)};
    glUniform2fv(g->glContext->screenSizeID, 1, &floatScreenSize.x);

    glm::mat4x4 mat = g->renderData->gameCamera.getProjectionMatrix(screenSize.x, screenSize.y);
    glUniformMatrix4fv(g->glContext->orthoProjectionID, 1, GL_FALSE, &mat[0].x);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * g->renderData->transformCount,
                    g->renderData->transforms);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g->renderData->transformCount);
    g->renderData->transformCount = 0;
}

void ui_render(GlobalState *g) {
    auto screenSize = g->appState->screenSize;

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4x4 mat = g->renderData->uiCamera.getProjectionMatrix(screenSize.x, screenSize.y);
    glUniformMatrix4fv(g->glContext->orthoProjectionID, 1, GL_FALSE, &mat[0].x);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    sizeof(Transform) * g->renderData->uiTransformCount,
                    g->renderData->uiTransforms);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g->renderData->uiTransformCount);

    // 42 w/o help 193 with it
    // log("%d", g->renderData->uiTransformCount);
    g->renderData->uiTransformCount = 0;

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
