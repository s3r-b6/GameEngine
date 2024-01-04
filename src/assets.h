#pragma once

#include "engine_lib.h"

struct Sprite {
    glm::ivec2 atlasOffset;
    glm::ivec2 spriteSize;

    GLuint atlasIdx;
};

enum SpriteID { Player, Chest };

Sprite get_sprite(SpriteID spriteID);
