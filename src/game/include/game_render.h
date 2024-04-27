#pragma once

#include <glad.h>

#include "assets.h"
#include "types.h"

void ui_drawTextFormatted(glm::vec2 pos, float fontSize, const char *text...);
void ui_drawText(glm::vec2 pos, float fontSize, const char *text);
void drawTileGroup(TileID t1, TileID t2, glm::ivec2 pos);
void ui_drawTileGroup(TileID t1, TileID t2, glm::ivec2 pos);
void UIdrawTileID(TileID t1, glm::vec2 pos);
void drawTileID(TileID tileId, glm::vec2 pos);

void drawSprite(SpriteID spriteID, glm::vec2 pos, glm::vec2 size);
void drawAnimatedSprite(AnimatedSpriteID spriteID, glm::vec2 pos, glm::vec2 size, int frame);

void drawTile(glm::vec2 tile, u8 atlasIdx, glm::vec2 pos);
void UIdrawTile(glm::vec2 tile, u8 atlasIdx, glm::vec2 pos);
