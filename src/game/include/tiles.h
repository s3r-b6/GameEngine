#pragma once

#include <map>
#include <vector>

#include "./engine_global.h"

#include "./engine_lib.h"
#include "./entities.h"
#include "./game_render.h"
#include "./input.h"
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
    TileID chunkTiles[TILES_CHUNK_x * TILES_CHUNK_x];
    std::vector<FrontTile> collisions;
};

// TODO: This is really bad. Should store n chunks instead of n individual tiles
struct TileManager {
    TileChunk world[16];

    u32 currentTiles = 1;
    std::map<int, TileBase> tilemap;

    TileID registerTile(TileBase t) {
        t.id = currentTiles;
        tilemap[currentTiles++] = t;
        return t.id;
    }

    void render() {
        for (int i = 0; i < TILES_CHUNK_x * TILES_CHUNK_y; i++) {
            TileID tileID = world[0].chunkTiles[i];
            // if (tileID == 0) continue;
            int x = i % TILES_CHUNK_x, y = i / TILES_CHUNK_x;
            drawTileID(tileID, {x * 16, y * 16});
        }
    }
};

// TODO:
bool checkTileCollisions(ColliderComponent *collider) { return false; }
