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
    static constexpr int gridSize = WORLD_SIZE_x * WORLD_SIZE_y;
    std::map<int, TileBase> tilemap;

    u32 currentTiles = 0;
    u8 shownAtlas = 2;

    Tile worldgrid[gridSize] = {0};
    std::vector<FrontTile> collisions;

    TileSelection selection;

    bool tilePickerShown;

    u16 registerTile(TileBase t) {
        t.id = currentTiles;
        tilemap[currentTiles++] = t;
        return t.id;
    }

    void clear() {
        for (int i = 0; i < gridSize; i++) {
            worldgrid[i] = {0};
        }

        collisions.clear();
    }

    // TODO: Should also store some metadata about the world (size, id, that kind of things)
    //
    // Idea: Save how many tiles and collisions there are at the top.
    bool serialize() {
        FILE *f = fopen("world.data", "wb");

        // Write gridSize tiles
        // engine_log("serializing %d tiles", gridSize);
        for (int i = 0; i < gridSize; i++) {
            Tile t1 = worldgrid[i];

            u32 data = t1.serialize();
            fwrite(&data, sizeof(u32), 1, f);
        }

        u64 size = collisions.size();
        // engine_log("serializing %lu", size);
        //  Write how many collisions there are
        fwrite(&size, sizeof(u64), 1, f);
        for (auto col : collisions) {
            u64 data = col.serialize();
            fwrite(&data, sizeof(u64), 1, f);
        }

        fclose(f);
        return true;
    }

    // Read how many tiles and collisions there are. Deserialize n tiles and N collisions
    bool deserialize() {
        FILE *f = fopen("world.data", "rb");

        if (!f) {
            engine_log("Failed to open world.data");
            return false;
        }

        engine_log("Found previous world.data, loading the map");

        // Deserialize gridSize tiles
        for (int i = 0; i < gridSize; i++) {
            u32 data;
            fread(&data, sizeof(u32), 1, f);
            worldgrid[i].deserialize(data);
        }

        u64 colCount;
        fread(&colCount, sizeof(u64), 1, f);
        for (int i = 0; i < colCount; i++) {
            u64 data;
            fread(&data, sizeof(u64), 1, f);
            FrontTile t = {};
            t.deserialize(data);
            collisions.push_back(t);
        }

        fclose(f);

        return true;
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

        ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, input->mouseWorldPos * 16);
    }

    void renderFront(RenderData *renderData) {
        if (tilePickerShown) {
            drawTilePicker(WORLD_ATLAS, (int)1440, 40);
            return;
        }

        for (int i = 0; i < gridSize; i++) {
            int x = i % WORLD_SIZE_x;
            int y = i / WORLD_SIZE_x;

            Tile t1 = worldgrid[i];
            if (t1.atlasIdx) {
                drawTile(renderData, t1.x, t1.y, t1.atlasIdx, {x * TILESIZE, y * TILESIZE});
            }
        }
    }

    void renderBack(RenderData *renderData) {
        if (tilePickerShown) { return; }
        for (auto coll : collisions) {
            if (coll.atlasIdx) {
                drawTile(renderData, coll.x, coll.y, coll.atlasIdx,
                         {coll.worldX * TILESIZE, coll.worldY * TILESIZE});
            }
        }
    }

    void removeTile(int x, int y, int selectedLayer) {
        if (selectedLayer == 0) {
            worldgrid[(y * WORLD_SIZE_x) + x].atlasIdx = 0;
        } else if (selectedLayer == 1) {
            // This might be slow, but this method is not part of the game
            // + there should not be many collisions
            for (auto it = collisions.begin(); it != collisions.end();) {
                if (it->worldX == x && it->worldY == y) {
                    it = collisions.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    void setTile(int x, int y, TileBase t, u8 layer) {
        local_persist float last_call = 0.f;

        if (layer == 0) {
            worldgrid[(y * WORLD_SIZE_x) + x] = Tile{t};
        } else {
            FrontTile tile = {};

            tile.worldX = (u16)x;
            tile.worldY = (u16)y;
            tile.x = (u8)(t.x);
            tile.y = (u8)(t.y);
            tile.atlasIdx = t.atlasIdx;

            collisions.push_back(tile);
        }
    }

    void setTile(int x, int y, Tile t, u8 layer) {
        local_persist float last_call = 0.f;

        if (layer == 0) {
            worldgrid[(y * WORLD_SIZE_x) + x] = t;
        } else {
            FrontTile tile = {};

            tile.worldX = (u16)x;
            tile.worldY = (u16)y;
            tile.x = (u8)(t.x);
            tile.y = (u8)(t.y);
            tile.atlasIdx = t.atlasIdx;

            collisions.push_back(tile);
        }
    }

    void setTiles(glm::vec2 pos, TileBase t1, TileBase t2, u8 layer) {
        for (int x = 0; x <= t2.x - t1.x; x++) {
            for (int y = 0; y <= t2.y - t1.y; y++) {
                int xPos = pos.x + x, yPos = pos.y + y;
                if (layer == 0) {
                    Tile tile = {};

                    tile.x = (u8)(t1.x + x);
                    tile.y = (u8)(t1.y + y);
                    tile.atlasIdx = t1.atlasIdx;

                    worldgrid[(yPos * WORLD_SIZE_x) + xPos] = tile;
                } else {
                    FrontTile tile = {};
                    tile.worldX = (u16)xPos;
                    tile.worldY = (u16)yPos;
                    tile.x = (u8)(t1.x + x);
                    tile.y = (u8)(t1.y + y);
                    tile.atlasIdx = t1.atlasIdx;

                    collisions.push_back(tile);
                }
            }
        }
    }

    void setTiles(glm::vec2 pos, Tile t1, Tile t2, u8 layer) {
        for (int x = 0; x <= t2.x - t1.x; x++) {
            for (int y = 0; y <= t2.y - t1.y; y++) {
                int xPos = pos.x + x, yPos = pos.y + y;
                if (layer == 0) {
                    Tile tile = {};

                    tile.x = (u8)(t1.x + x);
                    tile.y = (u8)(t1.y + y);
                    tile.atlasIdx = t1.atlasIdx;

                    worldgrid[(yPos * WORLD_SIZE_x) + xPos] = tile;
                } else {
                    FrontTile tile = {};
                    tile.worldX = (u16)xPos;
                    tile.worldY = (u16)yPos;
                    tile.x = (u8)(t1.x + x);
                    tile.y = (u8)(t1.y + y);
                    tile.atlasIdx = t1.atlasIdx;

                    collisions.push_back(tile);
                }
            }
        }
    }
};

bool checkTileCollisions(ColliderComponent *collider) {
    for (auto coll : gameState->tileManager->collisions) {
        if (collider->checkCollisions({coll.worldX * TILESIZE, coll.worldY * TILESIZE}, {16, 16}))
            return true;
    }

    return false;
}
