// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./assets.h"

Sprite get_sprite(SpriteID spriteID) {
    Sprite sprite = {};

    switch (spriteID) {
    case Player: {
        sprite.atlasIdx = 0;
        sprite.atlasOffset = {1, 6};
        sprite.spriteSize = {16, 32};
        break;
    };

    case Chest: {
        sprite.atlasIdx = 1;
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
