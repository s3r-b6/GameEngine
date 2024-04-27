#pragma once

#include "./types.h"

#define FONT_ATLAS 0
#define WORLD_ATLAS 1
#define OBJ_ATLAS 2
#define CHAR_ATLAS 3

struct Sprite {
    glm::ivec2 atlasOffset;
    glm::ivec2 spriteSize;

    u8 atlasIdx;
};

enum AnimatedSpriteID {
    PlayerD_Walk,
    PlayerR_Walk,
    PlayerL_Walk,
    PlayerU_Walk,

    PlayerD_Attack,
    PlayerU_Attack,
    PlayerR_Attack,
    PlayerL_Attack
};

enum SpriteID {
    INVALID,
    PlayerD,
    PlayerR,
    PlayerL,
    PlayerU,
    Chest,
    SPRITE_COUNT,
};

Sprite get_animated_sprite(AnimatedSpriteID animatedSprite, int frame);
Sprite get_sprite(SpriteID spriteID);
