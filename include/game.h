#pragma once

#include "./globals.h"
#include "./platform.h"
#include "./renderer.h"
#include "./types.h"

struct EntityManager;
struct TileManager;

enum GameAction {
    UNKNOWN,

    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    QUIT,

    TILE_1,
    TILE_2,
    TILE_3,

    GAME_ACTION_COUNT,
};

struct GameState {
    bool initialized;
    double updateTimer;
    SDL_Keycode gameBinds[GAME_ACTION_COUNT] = {0};

    EntityManager *entityManager;
    TileManager *tileManager;
};

struct Tile {
    u8 x, y;
    u8 atlasIdx; // An atlasIdx of 0 == an invalid tile or a non-tile
    u8 pad = 0;

    void deserialize(u32 data) {
        u8 *dataP = (u8 *)&data;

        SDL_Log("First byte        (x): %hx\n", dataP[3]);
        SDL_Log("Second byte       (y): %hx\n", dataP[2]);
        SDL_Log("Third byte (atlasIdx): %hx\n", dataP[1]);
        SDL_Log("Pad byte             : %hx\n", dataP[0]);

        x = dataP[3];
        y = dataP[2];
        atlasIdx = dataP[1];
    }

    u32 serialize() {
        u32 data = 0;
        data |= (u32)x << 24;
        data |= (u32)y << 16;
        data |= (u32)atlasIdx << 8;
        return data;
    }
};

global Tile selectedTile = {0};

struct TileManager {
    static constexpr int size = WORLD_SIZE_x * WORLD_SIZE_y;
    Tile worldGrid[size] = {0};

    // u32 serialize() {}

    void render() {
        for (int i = 0; i < size; i++) {
            int x = i % WORLD_SIZE_x;
            int y = i / WORLD_SIZE_x;

            Tile t = worldGrid[(y * WORLD_SIZE_x) + x];
            if (t.atlasIdx) {
                draw_tile(g->renderData, t.x, t.y, t.atlasIdx, {x * TILESIZE, y * TILESIZE});
            }
        }
    }

    void removeTile(int x, int y) { worldGrid[(y * WORLD_SIZE_x) + x].atlasIdx = 0; }
    void setTile(int x, int y, Tile t) {
        Tile tile = {};

        tile.x = t.x, tile.y = t.y;
        tile.atlasIdx = t.atlasIdx;
        worldGrid[(y * WORLD_SIZE_x) + x] = tile;
    }
};

extern "C" {
EXPORT_FN void updateGame(GlobalState *globalStateIn, float dt);
}

void simulate();
