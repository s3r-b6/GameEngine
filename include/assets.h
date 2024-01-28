#pragma once

#include "./types.h"

#define FONT_ATLAS 0
#define CHAR_ATLAS 1
#define OBJ_ATLAS 2
#define WORLD_ATLAS 3

struct Sprite {
    glm::ivec2 atlasOffset;
    glm::ivec2 spriteSize;

    u8 atlasIdx;
};

enum SpriteID {
    Player,
    Chest,
    SPRITE_COUNT,
};

enum TileID {
    Grass1,
    Water1,
    TILE_COUNT,
};

Sprite get_sprite(SpriteID spriteID);
Sprite get_tile(u8 x, u8 y, u8 atlasIdx);
