#include "game_render.h"

#include "globals.h"
#include "renderer.h"
#include "types.h"
#include <string>

#include <stdarg.h>
#include <stdio.h>
#include <string>

__attribute__((format(__printf__, 4, 5))) void
ui_drawTextFormatted(RenderData *renderData, glm::vec2 pos, float fontSize, const char *text...) {
    va_list args;
    va_start(args, text);

    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, text, argsCopy) + 1;
    va_end(argsCopy);

    std::string result(size, '\0');
    vsnprintf(&result[0], size, text, args);

    const char *char_ptr = result.c_str();
    ui_drawText(renderData, pos, fontSize, char_ptr);
}

void ui_drawText(RenderData *renderData, glm::vec2 pos, float fontSize, const char *text) {
    if (!text) { return; }

    glm::vec2 origin = pos;
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
        transform.size = glm::vec2(glyph.size) * fontSize;

        renderData->uiTransforms[renderData->uiTransformCount++] = transform;
        pos.x += glyph.advance.x * fontSize;
    }
}

void drawTileGroup(RenderData *renderData, glm::vec2 tile1, glm::vec2 tile2, u8 atlasIdx,
                   glm::ivec2 pos) {
    int xDiff = tile2.x - tile1.x + 1;
    int ySpan = tile2.y - tile1.y + 1;

    Sprite sp = getTile(tile1.x, tile1.y, atlasIdx);
    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = {xDiff * 16, ySpan * 16},
        .pos = pos,
        .size = {xDiff * 16, ySpan * 16},

        .atlasIdx = sp.atlasIdx,
    };

    renderData->transforms[renderData->transformCount++] = t;
}

void ui_drawTileGroup(RenderData *renderData, glm::vec2 tile1, glm::vec2 tile2, u8 atlasIdx,
                      glm::ivec2 pos) {
    int xDiff = tile2.x - tile1.x + 1;
    int ySpan = tile2.y - tile1.y + 1;

    Sprite sp = getTile(tile1.x, tile1.y, atlasIdx);
    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = {xDiff * 16, ySpan * 16},
        .pos = pos,
        .size = {xDiff * 16, ySpan * 16},

        .atlasIdx = sp.atlasIdx,
    };

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}

void ui_drawTile(RenderData *renderData, glm::vec2 tile, u8 atlasIdx, glm::vec2 pos) {
    Sprite sp = getTile(tile.x, tile.y, atlasIdx);

    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = sp.spriteSize,
        .pos = pos,
        .size = {16, 16},

        .atlasIdx = sp.atlasIdx,
    };

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}

void drawSprite(RenderData *renderData, SpriteID spriteID, glm::vec2 pos, glm::vec2 size) {
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

void drawAnimatedSprite(RenderData *renderData, AnimatedSpriteID spriteID, glm::vec2 pos,
                        glm::vec2 size, int frame) {
    Sprite sp = get_animated_sprite(spriteID, frame);

    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = sp.spriteSize,
        .pos = pos,
        .size = size,

        .atlasIdx = sp.atlasIdx,
    };

    renderData->transforms[renderData->transformCount++] = t;
}

void drawTile(RenderData *renderData, u8 x, u8 y, u8 atlasIdx, glm::vec2 pos) {
    Sprite sp = getTile(x, y, atlasIdx);

    Transform t = {
        .atlasOffset = sp.atlasOffset,
        .spriteSize = sp.spriteSize,
        .pos = pos,
        .size = {16, 16},

        .atlasIdx = sp.atlasIdx,
    };

    renderData->transforms[renderData->transformCount++] = t;
}
