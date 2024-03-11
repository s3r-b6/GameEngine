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

void render(GlobalState *g) {
    auto screenSize = g->appState->screenSize;
    auto color = g->renderData->clearColor;
    glClearColor(color[0], color[1], color[2], 1.f);
    glClearDepth(0.f);

    glViewport(0, 0, screenSize.x, screenSize.y);
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
    g->renderData->uiTransformCount = 0;

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}