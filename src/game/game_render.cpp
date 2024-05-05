#include <string>

#include "engine_global.h"
#include "game_render.h"
#include "renderer.h"
#include "tiles.h"

void UIdrawTextFormatted(glm::vec2 pos, float fontSize, const char *text...) {
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
    UIdrawText(pos, fontSize, char_ptr);
}

void UIdrawText(glm::vec2 pos, float fontSize, const char *text) {
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

void UIdrawTile(glm::vec2 tile, u8 atlasIdx, glm::vec2 pos) {
    Transform t = {};

    t.atlasOffset = {tile.x * 16, tile.y * 16};
    t.spriteSize = {16, 16};
    t.pos = pos;
    t.size = {16, 16};
    t.atlasIdx = atlasIdx;

    renderData->uiTransforms[renderData->uiTransformCount++] = t;
}

bool checkViewport(glm::vec2 pos) {
    // Left, Right, Top, Bottom
    local_persist float viewport[4] = {0.f, 0.f, 0.f, 0.f};
    local_persist float lastCameraPosX = 0.f;
    local_persist float lastCameraPosY = 0.f;

    if (lastCameraPosY != renderData->gameCamera.pos.y || // Last data is now invalid
        lastCameraPosX != renderData->gameCamera.pos.x || // OR
        viewport[0] == 0.f && viewport[1] == 0.f) {       // Not initialized
        viewport[0] = renderData->gameCamera.pos.x - CAMERA_SIZE_x * 0.55;
        viewport[1] = renderData->gameCamera.pos.x + renderData->gameCamera.dimensions.x;
        viewport[2] = renderData->gameCamera.pos.y - CAMERA_SIZE_y * 0.55;
        viewport[3] = renderData->gameCamera.pos.y + renderData->gameCamera.dimensions.y;
    }

    bool isWithinVp = pos.x >= viewport[0] && pos.x + TILESIZE <= viewport[1] &&
                      pos.y >= viewport[2] - TILESIZE && pos.y + TILESIZE <= viewport[3];

    return isWithinVp;
}

void drawSprite(SpriteID spriteID, glm::vec2 pos, glm::vec2 size) {
    if (!checkViewport(pos)) return;

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
    if (!checkViewport(pos)) return;

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
    if (!checkViewport(pos)) return;

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
    if (!checkViewport(pos)) return;

    Transform t = {};

    t.atlasOffset = {tile.x * 16, tile.y * 16};
    t.spriteSize = {16, 16};
    t.pos = pos;
    t.size = {16, 16};
    t.atlasIdx = atlasIdx;

    renderData->transforms[renderData->transformCount++] = t;
}
