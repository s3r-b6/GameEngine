// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./assets.h"
#include "engine_lib.h"

Sprite get_sprite(SpriteID spriteID) {
    Sprite sprite = {};

    switch (spriteID) {
    case Player: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.atlasOffset = {1, 6};
        sprite.spriteSize = {16, 32};
        break;
    };

    case Chest: {
        sprite.atlasIdx = OBJ_ATLAS;
        sprite.atlasOffset = {0, 0};
        sprite.spriteSize = {16, 16};
        break;
    };
    }

    return sprite;
}

Sprite get_tile(u8 x, u8 y, u8 atlasIdx) {
    Sprite sprite = {
        .atlasOffset = {x * 16, y * 16},
        .spriteSize = {16, 16},
        .atlasIdx = atlasIdx,
    };

    return sprite;
}

Sprite get_char(char ch) {
    if (ch < 32 || ch > 126) { crash("Non-ASCII not supported yet"); }

    Sprite sprite = {};

    // width = 512, height = 256
    // cellWidth and cellHeight = 32
    // fontSize 

    int idx = ch - 32;

    int xPos = (idx * 32) % 512;
    int yPos = (idx * 32) / 512 * 32;

    sprite.atlasOffset = {xPos, yPos};
    sprite.spriteSize = {32, 32};

    return sprite;
}
