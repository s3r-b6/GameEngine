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
        sprite.atlasOffset = {0 + (16 * (frame % 4)), 6};
        break;
    };

    case PlayerR_Walk: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {0 + (16 * (frame % 4)), 101};
        break;
    };
    case PlayerU_Walk: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {0 + (16 * (frame % 4)), 68};
        break;
    };
    case PlayerL_Walk: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {0 + (16 * (frame % 4)), 37};
        break;
    };

    case PlayerD_Attack: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {8 + (32 * (frame % 4)), 133};
        break;
    };

    case PlayerR_Attack: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {8 + (32 * (frame % 4)), 198};
        break;
    };
    case PlayerU_Attack: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {8 + (32 * (frame % 4)), 165};
        break;
    };
    case PlayerL_Attack: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {8 + (32 * (frame % 4)), 229};
        break;
    };

    default: {
        crash("Could not find the animated sprite");
        break;
    }
    }

    return sprite;
}

// This values are hardcoded by hand looking at the atlas
Sprite get_sprite(SpriteID spriteID) {
    Sprite sprite = {};

    switch (spriteID) {
    case PlayerD: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {0, 6};
        break;
    };

    case PlayerL: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {0, 101};
        break;
    };
    case PlayerU: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {0, 68};
        break;
    };
    case PlayerR: {
        sprite.atlasIdx = CHAR_ATLAS;
        sprite.spriteSize = {16, 32};
        sprite.atlasOffset = {0, 37};
        break;
    };

    case Chest: {
        sprite.atlasIdx = OBJ_ATLAS;
        sprite.atlasOffset = {0, 0};
        sprite.spriteSize = {16, 16};
        break;
    };
    case INVALID:
    case SPRITE_COUNT:
        break;
    }

    return sprite;
}
