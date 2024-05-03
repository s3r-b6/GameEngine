#pragma once

#include <map>
#include <vector>

#include "./entities.h"
#include "./renderer.h"
#include "./types.h"

struct TileBase {
    u8 x, y;     // The pos in the tilemap
    u8 atlasIdx; // An atlasIdx of 0 == an invalid tile or a non-tile
    TileID id;
};

struct FrontTile {
    u16 posX, posY;
    TileID id;
};

struct TileChunk {
    i16 x, y;
    TileID *chunkTiles;
    std::vector<FrontTile> frontTiles;
    std::vector<FrontTile> collisions;
};

struct TileManager {
    static constexpr u8 MAX_ROOMS = 4;
    TileChunk world[MAX_ROOMS];

    u32 currentTiles;
    std::map<int, TileBase> tilemap;

    TileID registerTile(TileBase t) {
        t.id = currentTiles;
        tilemap[currentTiles++] = t;
        return t.id;
    }

    void renderChunk(const TileChunk &chunk, float offset_x, float offset_y) {
        float viewport_left = renderData->gameCamera.pos.x - CAMERA_SIZE_x * 0.55;
        float viewport_right = renderData->gameCamera.pos.x + renderData->gameCamera.dimensions.x;
        float viewport_top = renderData->gameCamera.pos.y - CAMERA_SIZE_y * 0.55;
        float viewport_bottom = renderData->gameCamera.pos.y + renderData->gameCamera.dimensions.y;

        for (int i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) {
            TileID tileID = chunk.chunkTiles[i];
            if (tileID == MAX_U16) continue;

            // Calculate the position of the tile in world space
            int x = i % TILES_CHUNK_x, y = i / TILES_CHUNK_x;
            float tile_x = offset_x + x * TILESIZE;
            float tile_y = offset_y + y * TILESIZE;

            // Check if the tile is within the viewport
            if (tile_x >= viewport_left && tile_x + TILESIZE <= viewport_right &&
                tile_y >= viewport_top && tile_y + TILESIZE <= viewport_bottom) {
                drawTileID(tileID, {tile_x, tile_y});
            }
        }
    }

    void render() {
        for (auto &chunk : world) {
            if (chunk.x == MAX_I16 && chunk.y == MAX_I16) { continue; }
            float offsetX = chunk.x * TILES_CHUNK_x * TILESIZE,
                  offsetY = chunk.y * TILES_CHUNK_y * TILESIZE;

            // Chunks have their origin top left, but distance should be calculated from center
            glm::vec2 distVector = glm::vec2((offsetX * 1.5) - renderData->gameCamera.pos.x,
                                             (offsetY * 1.5) - renderData->gameCamera.pos.y);

            if (offsetX == 0) { distVector.x += CHUNK_SIZE_x / 2.f; }
            if (offsetY == 0) { distVector.y += CHUNK_SIZE_y / 2.f; }

            if (distVector.x > CHUNK_SIZE_x || distVector.y > CHUNK_SIZE_y) {
                // engine_log("%f %f %f %f", offsetX, offsetY, renderData->gameCamera.pos.x,
                //            renderData->gameCamera.pos.y);
                continue;
            }

            renderChunk(chunk, offsetX, offsetY);
        }
    }
};

// TODO:
bool checkTileCollisions(ColliderComponent *collider) { return false; }
