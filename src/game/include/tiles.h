#pragma once

#include <map>
#include <vector>

#include "./entities.h"
#include "./renderer.h"
#include "./types.h"
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
    u8 pad[2];

    void render(glm::vec2 offset) {}
};

struct AnimatedTile : PosTile {
    TileID *nextTiles;
    u8 tileCount;
    u8 currTile;
    u8 tilesPerSecond;
    double *dt;
    float timer;

    void render(glm::vec2 offset) {
        timer -= *dt;
        if (timer <= 0) {
            currTile = (currTile + 1) % tileCount;
            if (currTile >= tileCount) { currTile = 0; }
            timer = tilesPerSecond / TARGET_FPS;
        }

        // Draw the current tile at the specified position
        drawTileID((currTile == 0) ? id : nextTiles[currTile], {offset.x + posX, offset.y + posY});

        // engine_log("Timer: %f, Delta Time: %f, Current Tile: %u", timer, *dt, currTile);
        // engine_log("Total Tiles: %i", tileCount);
    }
};

// Chunks are just "rooms", so, instead of having their own memory to represent the world
// they share the same. I.e., if there are 2 rooms 1, we just have 2 ptrs to the room1 memory
struct TileChunk {
    i16 x, y;
    //  The worldgrid
    TileID *chunkTiles;
    //  The front layers
    PosTile *collisions;
    u8 pad1;
    PosTile *frontTiles;
    u8 pad2;
    //  Count of items in front layers
    u16 collisionCount, frontCount;
};

struct TileManager {
    static constexpr u8 MAX_ROOMS = 16;
    TileChunk world[MAX_ROOMS];

    TileID currentTiles = 0;
    std::map<TileID, TileBase> tilemap;

    void registerTile(TileBase t) { tilemap[currentTiles++] = t; }

    void renderWorldGrid(const TileChunk &chunk, float offset_x, float offset_y) {
        for (int i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) {
            TileID tile = chunk.chunkTiles[i];
            // The worldGrid may contain invalid tiles
            if (tile == MAX_U16) continue;

            // Calculate the position of the tile in world space
            int x = i % TILES_CHUNK_x, y = i / TILES_CHUNK_x;
            float tile_x = offset_x + x * TILESIZE;
            float tile_y = offset_y + y * TILESIZE;

            drawTileID(tile, {tile_x, tile_y});
        }
    }

    void renderFrontTiles(const TileChunk &chunk, float offset_x, float offset_y) {
        for (int i = 0; i < chunk.collisionCount; i++) {
            PosTile ftile = chunk.collisions[i];
            float tile_x = offset_x + ftile.posX * TILESIZE;
            float tile_y = offset_y + ftile.posY * TILESIZE;
            drawTileID(ftile.id, {tile_x, tile_y});
        }

        for (int i = 0; i < chunk.frontCount; i++) {
            PosTile ftile = chunk.frontTiles[i];
            float tile_x = offset_x + ftile.posX * TILESIZE;
            float tile_y = offset_y + ftile.posY * TILESIZE;
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

// TODO:
bool checkTileCollisions(ColliderComponent *collider) { return false; }
