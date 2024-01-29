// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include <string>

#include "./engine_lib.h"
#include "./globals.h"
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

void render(GlobalState *g) {
    auto screenSize = g->appState->screenSize;
    auto color = g->renderData->clearColor;
    glClearColor(color[0], color[1], color[2], 1.f);
    glClearDepth(0.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    render_ui(mat, g);

    SDL_GL_SwapWindow(g->appState->window);
}

void render_ui(glm::mat4x4 proj, GlobalState *g) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    sizeof(Transform) * g->renderData->uiTransformCount,
                    g->renderData->uiTransforms);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g->renderData->uiTransformCount);
    g->renderData->uiTransformCount = 0;

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void draw_ui_text_formatted(RenderData *renderData, vec2 pos, float fontSize, const char *text,
                            ...) {
    va_list args;
    va_start(args, text);

    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, text, argsCopy) + 1;
    va_end(argsCopy);

    std::string result(size, '\0');
    vsnprintf(&result[0], size, text, args);

    const char *char_ptr = result.c_str();
    draw_ui_text(renderData, pos, fontSize, char_ptr);
}

void draw_ui_text(RenderData *renderData, vec2 pos, float fontSize, const char *text) {
    if (!text) { return; }

    vec2 origin = pos;
    while (char c = *(text++)) {
        if (c == '\n') {
            pos.y += renderData->fontHeight * fontSize;
            pos.x = origin.x;
            continue;
        }

        Glyph glyph = renderData->glyphs[c];
        Transform transform = {};
        transform.pos.x = pos.x + glyph.offset.x * fontSize;
        transform.pos.y = pos.y - glyph.offset.y * fontSize;
        transform.atlasOffset = glyph.textureCoords;
        transform.spriteSize = glyph.size;
        transform.size = vec2(glyph.size) * fontSize;

        renderData->uiTransforms[renderData->uiTransformCount++] = transform;
        pos.x += glyph.advance.x * fontSize;
    }
}

void draw_tile_ui(RenderData *renderData, u8 x, u8 y, u8 atlasIdx, glm::vec2 pos) {
    Sprite sp = get_tile(x, y, atlasIdx);

    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = sp.spriteSize,
        .pos = pos,
        .size = {16, 16},

        .atlasIdx = sp.atlasIdx,
    };

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}