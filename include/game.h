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
    u8 atlasIdx;

    bool initialized = false;
};

global Tile selectedTile = {0};

struct TileManager {
    static constexpr int size = WORLD_SIZE_x * WORLD_SIZE_y;
    Tile worldGrid[size] = {0};

    void render() {
        for (int i = 0; i < size; i++) {
            int x = i % WORLD_SIZE_x;
            int y = i / WORLD_SIZE_x;

            Tile t = worldGrid[(y * WORLD_SIZE_x) + x];
            if (t.initialized) {
                draw_tile(g->renderData, t.x, t.y, t.atlasIdx, {x * TILESIZE, y * TILESIZE});
            }
        }
    }

    void removeTile(int x, int y) { worldGrid[(y * WORLD_SIZE_x) + x].initialized = false; }
    void setTile(int x, int y, Tile t) {
        Tile tile = {};

        tile.x = t.x;
        tile.y = t.y;
        tile.atlasIdx = t.atlasIdx;
        tile.initialized = true;
        worldGrid[(y * WORLD_SIZE_x) + x] = tile;
    }
};

extern "C" {
EXPORT_FN void updateGame(GlobalState *globalStateIn, float dt);
}

void simulate();
