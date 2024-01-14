#include "./assets.h"
#include "./platform.h"
#include "./types.h"

#include "./globals.h"
#include "./renderer.h"

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
};

struct Tile {
    TileID spriteID;
    uint atlasIdx;
};

struct TileManager {
    static constexpr int size = WORLD_SIZE.x * WORLD_SIZE.y;
    Tile *worldGrid[size] = {nullptr};

    void render() {
        for (int i = 0; i < size; i++) {
            int x = i % WORLD_SIZE.x;
            int y = i / WORLD_SIZE.x;

            Tile *t = worldGrid[(y * WORLD_SIZE.x) + x];
            if (t != nullptr) {
                draw_tile(g->renderData, t->spriteID, {x * TILESIZE, y * TILESIZE});
            }
        }
    }

    void removeTile(int x, int y) { worldGrid[(y * WORLD_SIZE.x) + x] = nullptr; }

    void setTile(int x, int y, Tile *t) { worldGrid[(y * WORLD_SIZE.x) + x] = t; }
};

extern "C" {
EXPORT_FN void updateGame(GlobalState *globalStateIn, float dt);
}

void simulate();
inline bool gameRegisterKey(GameAction action, SDL_Keycode kc);
inline bool actionDown(GameAction action);
inline bool actionUp(GameAction action);
