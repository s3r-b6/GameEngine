// Copyright (c) 2024 <Sergio Bermejo de las Heras>
// This code is subject to the MIT license.

#include "./assets.h"
#include "./engine_lib.h"

// This values are hardcoded by hand looking at the atlas
Sprite get_animated_sprite(AnimatedSpriteID animatedSprite, int frame) {
    Sprite sprite = {};

    switch (animatedSprite) {

    case PlayerD_Walk: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {1 + (16 * (frame % 4)), 6};
        break;
    };

    case PlayerR_Walk: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {1 + (16 * (frame % 4)), 37};
        break;
    };
    case PlayerU_Walk: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {1 + (16 * (frame % 4)), 68};
        break;
    };
    case PlayerL_Walk: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {1 + (16 * (frame % 4)), 101};
        break;
    };

    default: {
        crash("Could not find that animated sprite");
        break;
    }
    }

    return sprite;
}

// This values are hardcoded by hand looking at the atlas
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

Sprite getTile(u8 x, u8 y, u8 atlasIdx) {
    Sprite sprite = {
        .atlasOffset = {x * 16, y * 16},
        .spriteSize = {16, 16},
        .atlasIdx = atlasIdx,
    };

    return sprite;
}
