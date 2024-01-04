#pragma once

#include "types.h"

struct Sprite {
    glm::ivec2 atlasOffset;
    glm::ivec2 spriteSize;

    uint atlasIdx;
};

enum SpriteID { Player, Chest };

Sprite get_sprite(SpriteID spriteID);
