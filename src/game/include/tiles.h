#pragma once

#include <map>
#include <vector>

#include "./engine_global.h"
#include "./game_global.h"

#include "./engine_lib.h"
#include "./entities.h"
#include "./game_render.h"
#include "./input.h"

struct TileBase {
    u8 x, y;     // The pos in the tilemap
    u8 atlasIdx; // An atlasIdx of 0 == an invalid tile or a non-tile
    u8 id;
};

struct Tile : TileBase {
    bool deserialize(u32 data) {
        u8 *dataP = (u8 *)&data;
        if (!dataP[1]) { return false; } // If no-tile, skip

        x = dataP[3];
        y = dataP[2];
        atlasIdx = dataP[1];

        return true;
    }

    u32 serialize() {
        u32 data = 0;
        data |= (u32)x << 24;
        data |= (u32)y << 16;
        data |= (u32)atlasIdx << 8;
        return data;
    }
};

struct FrontTile : TileBase {
    u16 worldX, worldY;

    bool deserialize(u64 data) {
        u8 *dataP = (u8 *)&data;
        if (!dataP[1]) { return false; } // If no-tile, skip

        worldX = (u16)dataP[6];
        worldY = (u16)dataP[4];
        x = dataP[3];
        y = dataP[2];
        atlasIdx = dataP[1];

        // engine_log("deserializing: wX:%u wY:%u x:%u y:%u idx:%u", data, worldX, worldY, x, y,
        // atlasIdx);

        return true;
    }

    u64 serialize() {
        u64 data = 0;
        data |= (u64)worldX << 48;
        data |= (u64)worldY << 32;
        data |= (u64)x << 24;
        data |= (u64)y << 16;
        data |= (u64)atlasIdx << 8;
        return data;
    }
};

struct TileSelection {
    TileBase selectedTile1;
    TileBase selectedTile2;
    u8 selectedLayer;
};

// TODO: This is really bad. Should store n chunks instead of n individual tiles
struct TileManager {
    TileSelection selection;
    u32 currentTiles = 0;
    std::map<int, TileBase> tilemap;
    u8 shownAtlas = 2;
    bool tilePickerShown;
    u16 registerTile(TileBase t) {
        t.id = currentTiles;
        tilemap[currentTiles++] = t;
        return t.id;
    }

    void drawTilePicker(int textureAtlas, int maxTiles, int tilesPerRow) {
        for (auto const &[key, val] : tilemap) {
            if (val.atlasIdx < shownAtlas) continue;
            ui_drawTile(renderData, {val.x, val.y}, val.atlasIdx, {val.x * 16, val.y * 16});
            if (val.atlasIdx > shownAtlas) break;
        }

        if (selection.selectedTile2.atlasIdx) {
            int xTiles = selection.selectedTile2.x - selection.selectedTile1.x;
            int yTiles = selection.selectedTile2.y - selection.selectedTile1.y;

            for (int x = 0; x <= xTiles; x++) {
                for (int y = 0; y <= yTiles; y++) {
                    ui_drawTile(renderData, {39, 35}, textureAtlas,
                                {(selection.selectedTile1.x + x) * 16,
                                 (selection.selectedTile1.y + y) * 16});
                }
            }
        } else {
            ui_drawTile(renderData, {39, 35}, textureAtlas,
                        {selection.selectedTile1.x * 16, selection.selectedTile1.y * 16});
        }

        ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, input->mouseUiPos * 16);
    }
};

bool checkTileCollisions(ColliderComponent *collider) { return false; }
