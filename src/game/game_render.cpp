#include "game_render.h"

#include "engine_global.h"
#include "renderer.h"
#include "types.h"
#include <string>

void ui_drawTextFormatted(glm::vec2 pos, float fontSize, const char *text...) {
    va_list args;
    va_start(args, text);

    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, text, argsCopy) + 1;
    va_end(argsCopy);

    std::string result(size, '\0');
    vsnprintf(&result[0], size, text, args);
    va_end(args);

    const char *char_ptr = result.c_str();
    ui_drawText(pos, fontSize, char_ptr);
}

void ui_drawText(glm::vec2 pos, float fontSize, const char *text) {
    if (!text) { return; }

    glm::vec2 origin = pos;
    while (char c = *(text++)) {
        if (c == '\n') {
            pos.y += renderData->fontHeight * fontSize;
            pos.x = origin.x;
            continue;
        }

        Glyph glyph = renderData->glyphs[c];
        Transform t = {};

        t.atlasOffset = glyph.textureCoords;
        t.spriteSize = glyph.size;
        t.pos = {pos.x + glyph.offset.x * fontSize, pos.y - glyph.offset.y * fontSize};
        t.size = glm::vec2(glyph.size) * fontSize;
        t.atlasIdx = FONT_ATLAS;

        renderData->uiTransforms[renderData->uiTransformCount++] = t;
        pos.x += glyph.advance.x * fontSize;
    }
}

void drawTileGroup(glm::vec2 tile1, glm::vec2 tile2, u8 atlasIdx, glm::ivec2 pos) {
    int xDiff = tile2.x - tile1.x + 1;
    int ySpan = tile2.y - tile1.y + 1;

    Sprite sp = getTile(tile1.x, tile1.y, atlasIdx);
    Transform t = {};
    t.atlasOffset = sp.atlasOffset;
    t.spriteSize = {xDiff * 16, ySpan * 16};
    t.pos = pos;
    t.size = {xDiff * 16, ySpan * 16};
    t.atlasIdx = sp.atlasIdx,

    renderData->transforms[renderData->transformCount++] = t;
}

void ui_drawTileGroup(glm::vec2 tile1, glm::vec2 tile2, u8 atlasIdx, glm::ivec2 pos) {
    int xDiff = tile2.x - tile1.x + 1;
    int ySpan = tile2.y - tile1.y + 1;

    Sprite sp = getTile(tile1.x, tile1.y, atlasIdx);
    Transform t = {};

    t.atlasOffset = sp.atlasOffset;
    t.spriteSize = {xDiff * 16, ySpan * 16};
    t.pos = pos;
    t.size = {xDiff * 16, ySpan * 16};

    t.atlasIdx = sp.atlasIdx;

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}

void ui_drawTile(glm::vec2 tile, u8 atlasIdx, glm::vec2 pos) {
    Sprite sp = getTile(tile.x, tile.y, atlasIdx);

    Transform t = {};

    t.atlasOffset = sp.atlasOffset;
    t.spriteSize = sp.spriteSize;
    t.pos = pos;
    t.size = {16, 16};

    t.atlasIdx = sp.atlasIdx;

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}

void drawSprite(SpriteID spriteID, glm::vec2 pos, glm::vec2 size) {
    Sprite sp = get_sprite(spriteID);

    Transform t = {};

    t.atlasOffset = sp.atlasOffset;
    t.spriteSize = sp.spriteSize;
    t.pos = pos;
    t.size = size;

    t.atlasIdx = sp.atlasIdx;

    renderData->transforms[renderData->transformCount++] = t;
}

void drawAnimatedSprite(AnimatedSpriteID spriteID, glm::vec2 pos, glm::vec2 size, int frame) {
    Sprite sp = get_animated_sprite(spriteID, frame);

    Transform t = {};

    t.atlasOffset = sp.atlasOffset;
    t.spriteSize = sp.spriteSize;
    t.pos = pos;
    t.size = size;

    t.atlasIdx = sp.atlasIdx;

    renderData->transforms[renderData->transformCount++] = t;
}

void drawTile(u8 x, u8 y, u8 atlasIdx, glm::vec2 pos) {
    Sprite sp = getTile(x, y, atlasIdx);

    Transform t = {};

    t.atlasOffset = sp.atlasOffset;
    t.spriteSize = sp.spriteSize;
    t.pos = pos;
    t.size = {16, 16};

    t.atlasIdx = sp.atlasIdx;

    renderData->transforms[renderData->transformCount++] = t;
}
