// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./assets.h"

Sprite get_sprite(SpriteID spriteID) {
    Sprite sprite = {};

    switch (spriteID) {
    case Player: {
        sprite.atlasIdx = 0;
        sprite.atlasOffset = {1, 6};
        sprite.spriteSize = {15, 22};
        break;
    };

    case Chest: {
        sprite.atlasIdx = 1;
        sprite.atlasOffset = {0, 0};
        sprite.spriteSize = {16, 16};
        break;
    };

    case Grass1: {
        sprite.atlasIdx = 2;
        sprite.atlasOffset = {0, 0};
        sprite.spriteSize = {16, 16};
        break;
    };

    case Water1: {
        sprite.atlasIdx = 2;
        sprite.atlasOffset = {0, 16};
        sprite.spriteSize = {16, 16};
        break;
    };
    }

    return sprite;
}
