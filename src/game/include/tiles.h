#pragma once

#include <map>
#include <vector>

#include "./engine_global.h"

#include "./engine_lib.h"
#include "./entities.h"
#include "./game_render.h"
#include "./input.h"

struct TileBase {
    u8 x, y;     // The pos in the tilemap
    u8 atlasIdx; // An atlasIdx of 0 == an invalid tile or a non-tile
    u8 id;
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

    void drawTilePicker() {
        for (auto const &[key, val] : tilemap) {
            if (val.atlasIdx < shownAtlas || val.atlasIdx == 0) continue;
            if (val.atlasIdx > shownAtlas) break;
            ui_drawTile({val.x, val.y}, val.atlasIdx, {val.x * 16, val.y * 16});
        }

        if (!selection.selectedTile2.atlasIdx) {
            ui_drawTile({39, 35}, WORLD_ATLAS,
                        {selection.selectedTile1.x * 16, selection.selectedTile1.y * 16});
        } else {
            int xTiles = selection.selectedTile2.x - selection.selectedTile1.x;
            int yTiles = selection.selectedTile2.y - selection.selectedTile1.y;

            for (int x = 0; x <= xTiles; x++) {
                for (int y = 0; y <= yTiles; y++) {
                    ui_drawTile({39, 35}, WORLD_ATLAS,
                                {(selection.selectedTile1.x + x) * 16,
                                 (selection.selectedTile1.y + y) * 16});
                }
            }
        }

        ui_drawTile({39, 35}, WORLD_ATLAS, input->mouseUiPos * 16);
    }

    void render() {
        if (tilePickerShown) {
            drawTilePicker();
            return;
        }

        // render...
    }
};

bool checkTileCollisions(ColliderComponent *collider) { return false; }
