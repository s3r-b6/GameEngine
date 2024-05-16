#pragma once

#include <map>
#include <vector>

#include "./entities.h"
#include "./renderer.h"
#include "./types.h"
#include "engine_global.h"
#include "engine_lib.h"
#include "game_render.h"

// The simplest tile. Its position is determined by its pos in
// the world grid. Created when parsing the tilemap png and put
// into a map. x and y are the position in the tilemap, atlasIdx
// is the id of the atlas
struct TileBase {
    u16 x, y;
    u16 atlasIdx;
};

// These tiles are meant to be used in a layer in front of the world
// grid. id maps the tile to a TileBase. posX and posY are the position
// of this tile inside the chunk.
// I.e., in world space, the tile is located at chunkOffset + tilePos
struct PosTile {
    TileID id;
    u16 posX, posY;
};

// TODO: Export animated tiles from the .tsx
struct AnimatedTile {
    u8 tileCount, duration;
    u8 pad[1];
    TileID *nextTiles;

    void render(double dt, glm::vec2 offset) {
        local_persist u8 currTile = 0;
        local_persist float timer = duration / TARGET_FPS;
        local_persist u64 prevFrame = 0;

        if (prevFrame != frame) {
            timer -= dt;
            if (timer <= 0) {
                if (++currTile >= tileCount) currTile = 0;
                timer = duration / TARGET_FPS;
            }
        }

        prevFrame = frame;

        // engine_log("%f %u %u %lu %lu", timer, currTile, tileCount, prevFrame, frame);
        drawTileID(nextTiles[currTile], {offset.x, offset.y});
    }
};

// Chunks are just "rooms", so, instead of having their own memory to represent the world
// they share the same. I.e., if there are 2 rooms 1, we just have 2 ptrs to the room1 memory
struct TileChunk {
    i16 x, y;
    //  The worldgrid
    TileID *chunkTiles;
    //  The front layers
    PosTile *ground2;
    u8 pad;
    PosTile *collisions;
    u8 pad2;
    PosTile *frontTiles;
};

struct TileManager {
    TileChunk world[MAX_ROOMS];

    TileID currentTiles = 0;
    std::map<TileID, TileBase> tilemap;
    std::map<TileID, AnimatedTile> animatedTiles;
    double *dt;

    void registerTile(TileBase t) { tilemap[++currentTiles] = t; }

    void renderWorldGrid(const TileChunk &chunk, float offset_x, float offset_y) {
        for (int i = 0;; i++) {
            PosTile ftile = chunk.ground2[i];

            if (ftile.id == MAX_U16) break;

            float tile_x = offset_x + ftile.posX;
            float tile_y = offset_y + ftile.posY;

            if (animatedTiles.count(ftile.id) == 1) {
                AnimatedTile &t = animatedTiles[ftile.id];
                t.render(*dt, {tile_x, tile_y});
                continue;
            }

            drawTileID(ftile.id, {tile_x, tile_y});
        }

        for (int i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) {
            TileID tile = chunk.chunkTiles[i];
            // The worldGrid could (?) contain invalid tiles
            if (tile == MAX_U16) continue;

            // Calculate the position of the tile in world space
            int x = i % TILES_CHUNK_x, y = i / TILES_CHUNK_x;
            float tile_x = offset_x + x * TILESIZE;
            float tile_y = offset_y + y * TILESIZE;

            if (animatedTiles.count(tile) == 1) {
                AnimatedTile &t = animatedTiles[tile];
                t.render(*dt, {tile_x, tile_y});
                continue;
            }

            drawTileID(tile, {tile_x, tile_y});
        }
    }

    void renderFrontTiles(const TileChunk &chunk, float offset_x, float offset_y) {
        for (int i = 0;; i++) {
            PosTile ftile = chunk.collisions[i];

            if (ftile.id == MAX_U16) break;

            float tile_x = offset_x + ftile.posX;
            float tile_y = offset_y + ftile.posY;

            if (animatedTiles.count(ftile.id) == 1) {
                AnimatedTile &t = animatedTiles[ftile.id];
                t.render(*dt, {tile_x, tile_y});
                continue;
            }

            drawTileID(ftile.id, {tile_x, tile_y});
        }

        for (int i = 0;; i++) {
            PosTile ftile = chunk.frontTiles[i];

            if (ftile.id == MAX_U16) break;

            float tile_x = offset_x + ftile.posX;
            float tile_y = offset_y + ftile.posY;

            if (animatedTiles.count(ftile.id) == 1) {
                AnimatedTile &t = animatedTiles[ftile.id];
                t.render(*dt, {tile_x, tile_y});
                continue;
            }

            drawTileID(ftile.id, {tile_x, tile_y});
        }
    }

    void renderBack() {
        for (auto &chunk : world) {
            if (chunk.x == MAX_I16 && chunk.y == MAX_I16) { continue; }
            float offsetX = chunk.x * TILES_CHUNK_x * TILESIZE,
                  offsetY = chunk.y * TILES_CHUNK_y * TILESIZE;

            // Chunks have their origin top left, but distance should be calculated from center
            glm::vec2 distVector = glm::vec2((offsetX * 1.5) - renderData->gameCamera.pos.x,
                                             (offsetY * 1.5) - renderData->gameCamera.pos.y);

            if (offsetX == 0) { distVector.x += CHUNK_SIZE_x / 2.f; }
            if (offsetY == 0) { distVector.y += CHUNK_SIZE_y / 2.f; }

            if (distVector.x > CHUNK_SIZE_x || distVector.y > CHUNK_SIZE_y) { continue; }

            renderWorldGrid(chunk, offsetX, offsetY);
        }
    }

    void renderFront() {
        for (auto &chunk : world) {
            if (chunk.x == MAX_I16 && chunk.y == MAX_I16) { continue; }
            float offsetX = chunk.x * TILES_CHUNK_x * TILESIZE,
                  offsetY = chunk.y * TILES_CHUNK_y * TILESIZE;

            // Chunks have their origin top left, but distance should be calculated from center
            glm::vec2 distVector = glm::vec2((offsetX * 1.5) - renderData->gameCamera.pos.x,
                                             (offsetY * 1.5) - renderData->gameCamera.pos.y);

            if (offsetX == 0) { distVector.x += CHUNK_SIZE_x / 2.f; }
            if (offsetY == 0) { distVector.y += CHUNK_SIZE_y / 2.f; }

            if (distVector.x > CHUNK_SIZE_x || distVector.y > CHUNK_SIZE_y) { continue; }

            renderFrontTiles(chunk, offsetX, offsetY);
        }
    }
};

inline bool checkTileCollisions(ColliderComponent *collider) {
    float x = collider->transform->pos.x, y = collider->transform->pos.y;

    for (int i = 0; i < MAX_ROOMS; i++) {
        TileChunk chunk = tileManager->world[i];

        float offsetX = chunk.x * TILES_CHUNK_x * TILESIZE,
              offsetY = chunk.y * TILES_CHUNK_y * TILESIZE;

        bool isInside = (x >= offsetX && x <= offsetX + CHUNK_SIZE_x && y >= offsetY &&
                         y <= offsetY + CHUNK_SIZE_y);

        if (!isInside) { continue; }

        for (int j = 0;; j++) {
            PosTile ftile = chunk.collisions[j];

            if (ftile.id == MAX_U16) break;

            float tile_x = offsetX + ftile.posX;
            float tile_y = offsetY + ftile.posY;
            if (collider->checkCollisions({tile_x, tile_y}, {16.f, 16.f})) return true;
        }
    }

    return false;
}
