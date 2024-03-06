#pragma once

#include <cstdio>

#include "SDL2/SDL_keyboard.h"

#include "./platform.h"
#include "./types.h"

#include "./game_render.h"

struct EntityManager;
struct TileManager;

enum GameAction {
    UNKNOWN,

    HELP,

    MOVE_U,
    MOVE_D,
    MOVE_L,
    MOVE_R,
    QUIT,

    TILE_1,
    TILE_2,
    TILE_3,

    LAYER_FRONT,
    LAYER_BACK,

    SAVE_WORLD,
    DELETE_WORLD,
    RELOAD_WORLD,

    GAME_ACTION_COUNT,
};

struct Tile {
    u8 x, y;
    u8 atlasIdx; // An atlasIdx of 0 == an invalid tile or a non-tile
    u8 pad = 0;

    bool deserialize(u32 data) {
        u8 *dataP = (u8 *)&data;
        if (!dataP[1]) { return false; } // If no-tile, skip

        // SDL_Log("First byte        (x): %hx\n", dataP[3]);
        // SDL_Log("Second byte       (y): %hx\n", dataP[2]);
        // SDL_Log("Third byte (atlasIdx): %hx\n", dataP[1]);
        // SDL_Log("Pad byte             : %hx\n", dataP[0]);

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

struct TileSelection {
    Tile selectedTile1;
    Tile selectedTile2;
};

struct GameState {
    bool initialized;
    SDL_Scancode gameBinds[GAME_ACTION_COUNT] = {};
    bool actionsTriggered[GAME_ACTION_COUNT] = {false};

    EntityManager *entityManager;
    TileManager *tileManager;

    TileSelection selection = {0};

    inline void gameRegisterKey(GameAction action, SDL_Keycode kc) {
        gameBinds[action] = SDL_GetScancodeFromKey(kc);
    }
};

struct TileManager {
    static constexpr int size = WORLD_SIZE_x * WORLD_SIZE_y;
    Tile worldGridLayer0[size] = {0};
    Tile worldGridLayer1[size] = {0};

    void clear() {
        for (int i = 0; i < size; i++) {
            worldGridLayer0[i] = {0};
            worldGridLayer1[i] = {0};
        }
    }

    bool deserialize() {
        FILE *f = fopen("world.data", "rb");

        if (!f) { return false; }

        for (int i = 0; i < size; i++) {
            int x = i % WORLD_SIZE_x;
            int y = i / WORLD_SIZE_x;
            Tile *t = &worldGridLayer0[i];

            u32 data;
            fread(&data, sizeof(u32), 1, f);
            if (t->deserialize(data)) {
                // SDL_Log("Tile {%d, %d} data: %d %d %d", x, y, t->x, t->y, t->atlasIdx);
            }

            t = &worldGridLayer1[i];
            fread(&data, sizeof(u32), 1, f);
            if (t->deserialize(data)) {
                // SDL_Log("Tile {%d, %d} data: %d %d %d", x, y, t->x, t->y, t->atlasIdx);
            }
        }

        fclose(f);

        return true;
    }

    // TODO: Should also store some metadata about the world (size, id, that kind of things)
    bool serialize() {
        FILE *f = fopen("world.data", "wb");

        for (int i = 0; i < size; i++) {
            Tile t1 = worldGridLayer0[i];
            Tile t2 = worldGridLayer1[i];

            u32 data = t1.serialize();
            fwrite(&data, sizeof(u32), 1, f);
            data = t2.serialize();
            fwrite(&data, sizeof(u32), 1, f);
        }

        fclose(f);
        return true;
    }

    void renderFront(RenderData *renderData) {
        for (int i = 0; i < size; i++) {
            int x = i % WORLD_SIZE_x;
            int y = i / WORLD_SIZE_x;

            Tile t1 = worldGridLayer0[i];
            if (t1.atlasIdx) {
                drawTile(renderData, t1.x, t1.y, t1.atlasIdx, {x * TILESIZE, y * TILESIZE});
            }
        }
    }

    void renderBack(RenderData *renderData) {
        for (int i = 0; i < size; i++) {
            int x = i % WORLD_SIZE_x;
            int y = i / WORLD_SIZE_x;
            Tile t2 = worldGridLayer1[i];
            if (t2.atlasIdx) {
                drawTile(renderData, t2.x, t2.y, t2.atlasIdx, {x * TILESIZE, y * TILESIZE});
            }
        }
    }

    void removeTile(int x, int y, int selectedLayer) {
        if (selectedLayer == 0) {
            worldGridLayer0[(y * WORLD_SIZE_x) + x].atlasIdx = 0;
        } else {
            worldGridLayer1[(y * WORLD_SIZE_x) + x].atlasIdx = 0;
        }
    }
    void setTile(int x, int y, Tile t, u8 layer) {
        Tile tile = {};

        tile.x = t.x, tile.y = t.y;
        tile.atlasIdx = t.atlasIdx;
        if (layer == 0) {
            worldGridLayer0[(y * WORLD_SIZE_x) + x] = tile;
        } else {
            worldGridLayer1[(y * WORLD_SIZE_x) + x] = tile;
        }
    }

    void setTiles(glm::vec2 pos, Tile t1, Tile t2, u8 layer) {
        for (int x = 0; x <= t2.x - t1.x; x++) {
            for (int y = 0; y <= t2.y - t1.y; y++) {
                Tile tile = {};
                tile.x = t1.x + x, tile.y = t1.y + y;
                tile.atlasIdx = t1.atlasIdx;

                int xPos = pos.x + x, yPos = pos.y + y;

                if (layer == 0) {
                    worldGridLayer0[(yPos * WORLD_SIZE_x) + xPos] = tile;
                } else {
                    worldGridLayer1[(yPos * WORLD_SIZE_x) + xPos] = tile;
                }
            }
        }
    }
};

extern "C" {
EXPORT_FN void updateGame(BumpAllocator *permStorage, BumpAllocator *tempStorage,
                          GlobalState *globalStateIn, float dt);
void drawTilePicker();
}

void drawTilePicker(int textureAtlas, int maxTiles, int tilesPerRow);
void handleInput();
void simulate();

void initializeGameState();
