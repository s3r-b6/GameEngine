#include <string>

#include "engine_global.h"
#include "game_render.h"
#include "renderer.h"
#include "tiles.h"

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

void drawTileGroup(TileID t1, TileID t2, glm::ivec2 pos) {
    TileBase tile1 = tileManager->tilemap[t1];
    TileBase tile2 = tileManager->tilemap[t2];

    int xDiff = tile2.x - tile1.x + 1;
    int ySpan = tile2.y - tile1.y + 1;

    Transform t = {
        .atlasOffset = {tile1.x * 16, tile1.y * 16},
        .spriteSize = {xDiff * 16, ySpan * 16},
        .pos = pos,
        .size = {xDiff * 16, ySpan * 16},

        .atlasIdx = tile1.atlasIdx,
    };

    renderData->transforms[renderData->transformCount++] = t;
}
void ui_drawTileGroup(TileID t1, TileID t2, glm::ivec2 pos) {
    TileBase tile1 = tileManager->tilemap[t1];
    TileBase tile2 = tileManager->tilemap[t2];

    int xDiff = tile2.x - tile1.x + 1;
    int ySpan = tile2.y - tile1.y + 1;

    Transform t = {
        .atlasOffset = {tile1.x * 16, tile1.y * 16},
        .spriteSize = {xDiff * 16, ySpan * 16},
        .pos = pos,
        .size = {xDiff * 16, ySpan * 16},

        .atlasIdx = tile1.atlasIdx,
    };

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}
void UIdrawTileID(TileID t1, glm::vec2 pos) {
    TileBase t = tileManager->tilemap[t1];

    Transform transform = {};
    transform.atlasOffset = {t.x * 16, t.y * 16};
    transform.spriteSize = {16, 16};
    transform.size = {16, 16};
    transform.atlasIdx = t.atlasIdx;
    transform.pos = pos;

    renderData->uiTransforms[renderData->uiTransformCount++] = transform;
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

void drawTileID(TileID tileID, glm::vec2 pos) {
    TileBase t = tileManager->tilemap[tileID];

    Transform transform = {};

    transform.atlasOffset = {t.x * 16, t.y * 16};
    transform.spriteSize = {16, 16};
    transform.size = {16, 16};
    transform.atlasIdx = t.atlasIdx;
    transform.pos = pos;

    renderData->transforms[renderData->transformCount++] = transform;
}

void drawTile(glm::vec2 tile, u8 atlasIdx, glm::vec2 pos) {
    Transform t = {};

    t.atlasOffset = {tile.x * 16, tile.y * 16};
    t.spriteSize = {16, 16};
    t.pos = pos;
    t.size = {16, 16};
    t.atlasIdx = atlasIdx;

    renderData->transforms[renderData->transformCount++] = t;
}

void UIdrawTile(glm::vec2 tile, u8 atlasIdx, glm::vec2 pos) {
    Transform t = {};

    t.atlasOffset = {tile.x * 16, tile.y * 16};
    t.spriteSize = {16, 16};
    t.pos = pos;
    t.size = {16, 16};
    t.atlasIdx = atlasIdx;

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}
