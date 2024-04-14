#pragma once

#include <vector>

#include "engine_lib.h"
#include "game_render.h"

struct Tile {
    u8 x, y;
    u8 atlasIdx; // An atlasIdx of 0 == an invalid tile or a non-tile
    u8 padding;

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

struct FrontTile {
    u16 worldX, worldY;
    u8 x, y, atlasIdx;
    u8 pad;

    bool deserialize(u64 data) {
        u8 *dataP = (u8 *)&data;
        if (!dataP[1]) { return false; } // If no-tile, skip

        worldX = (u16)dataP[6];
        worldY = (u16)dataP[4];
        x = dataP[3];
        y = dataP[2];
        atlasIdx = dataP[1];

        engine_log("deserializing: wX:%u wY:%u x:%u y:%u idx:%u", data, worldX, worldY, x, y,
                   atlasIdx);

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
    Tile selectedTile1;
    Tile selectedTile2;
};

// TODO: This is really bad. Should store n chunks instead of n individual tiles
struct TileManager {
    static constexpr int gridSize = WORLD_SIZE_x * WORLD_SIZE_y;
    Tile worldgrid[gridSize] = {0};
    std::vector<FrontTile> collisions;

    TileSelection selection;

    bool tilePickerShown;

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
        engine_log("serializing %d tiles", gridSize);
        for (int i = 0; i < gridSize; i++) {
            Tile t1 = worldgrid[i];

            u32 data = t1.serialize();
            fwrite(&data, sizeof(u32), 1, f);
        }

        u64 size = collisions.size();
        engine_log("serializing %lu", size);
        // Write how many collisions there are
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
        for (int i = 0; i < maxTiles; i++) {
            int x = i % tilesPerRow, y = i / tilesPerRow;

            int worldPosX = i * TILESIZE % WORLD_SIZE_x;
            int worldPosY = i * TILESIZE / WORLD_SIZE_x * TILESIZE;
            ui_drawTile(renderData, {x, y}, textureAtlas, {worldPosX, worldPosY});
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

        ui_drawTile(renderData, {39, 35}, WORLD_ATLAS, g->input->mouseUIpos * TILESIZE);
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

    void setTile(int x, int y, Tile t, u8 layer) {
        local_persist float last_call = 0.f;

        if (layer == 0) {
            worldgrid[(y * WORLD_SIZE_x) + x] = t;
        } else {
            FrontTile tile = {
                .worldX = (u16)x,
                .worldY = (u16)y,
                .x = (u8)(t.x),
                .y = (u8)(t.y),
                .atlasIdx = t.atlasIdx,
            };

            collisions.push_back(tile);
        }
    }

    void setTiles(glm::vec2 pos, Tile t1, Tile t2, u8 layer) {
        for (int x = 0; x <= t2.x - t1.x; x++) {
            for (int y = 0; y <= t2.y - t1.y; y++) {
                int xPos = pos.x + x, yPos = pos.y + y;
                if (layer == 0) {
                    Tile tile = {
                        .x = (u8)(t1.x + x),
                        .y = (u8)(t1.y + y),
                        .atlasIdx = t1.atlasIdx,
                    };

                    worldgrid[(yPos * WORLD_SIZE_x) + xPos] = tile;
                } else {
                    FrontTile tile = {
                        .worldX = (u16)xPos,
                        .worldY = (u16)yPos,
                        .x = (u8)(t1.x + x),
                        .y = (u8)(t1.y + y),
                        .atlasIdx = t1.atlasIdx,
                    };

                    collisions.push_back(tile);
                }
            }
        }
    }
};