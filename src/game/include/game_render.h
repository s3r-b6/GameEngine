#pragma once

#include <glad.h>

#include "assets.h"
#include "types.h"

void ui_drawTextFormatted(RenderData *renderData, glm::vec2 pos, float fontSize, const char *text,
                          ...);
void ui_drawText(RenderData *renderData, glm::vec2 pos, float fontSize, const char *text);
void drawTileGroup(RenderData *renderData, glm::vec2 tile1, glm::vec2 tile2, u8 atlasIdx,
                   glm::ivec2 pos);
void ui_drawTileGroup(RenderData *renderData, glm::vec2 tile1, glm::vec2 tile2, u8 atlasIdx,
                      glm::ivec2 pos);
void ui_drawTile(RenderData *renderData, glm::vec2 tile, u8 atlasIdx, glm::vec2 pos);
void drawTile(RenderData *renderData, u8 x, u8 y, u8 atlasIdx, glm::vec2 pos);
void drawSprite(RenderData *renderData, SpriteID spriteID, glm::vec2 pos, glm::vec2 size);
void drawAnimatedSprite(RenderData *renderData, AnimatedSpriteID spriteID, glm::vec2 pos,
                        glm::vec2 size, int frame);
