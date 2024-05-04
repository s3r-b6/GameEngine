
#include "./rooms.h"
#include "./tiles.h"

#include <delaunator.hpp>

_global u16 *room1Mem;
_global std::vector<FrontTile> *room1ColMem;

void placeRoom(Direction dirToParent, int x, int y) {
    local_persist int count = -1;
    if (count >= TileManager::MAX_ROOMS) { return; }

    if (count > 0) {
        for (auto &chunk : tileManager->world) {
            if (chunk.x == x && chunk.y == y) return;
        }
    }

    ++count;

    engine_log("%d Placing room on %d %d", count, x, y);
    tileManager->world[count].x = x, tileManager->world[count].y = y;
    tileManager->world[count].chunkTiles = room1Mem;
    tileManager->world[count].collisions = room1ColMem;

    Direction directions[] = {Direction::U, Direction::L, Direction::D, Direction::R};

    // reorder directions
    for (int i = 3; i > 0; --i) {
        int j = rand() % (i + 1);
        auto tmp = directions[i];
        directions[i] = directions[j];
        directions[j] = tmp;
    }

    int placed = 0;
    for (const auto &dir : directions) {
        if (dir != dirToParent) {
            switch (dir) {
            case Direction::U: placeRoom(dir, x, y - 1); break;
            case Direction::L: placeRoom(dir, x - 1, y); break;
            case Direction::D: placeRoom(dir, x, y + 1); break;
            case Direction::R: placeRoom(dir, x + 1, y); break;
            }
            ++placed;
            if ((dirToParent == Direction::No && placed >= 4) || (placed >= 3)) break;
        }
    }
}

void initRooms() {
    room1Mem = loadRoom(permStorage);
    room1ColMem = loadCollisions(permStorage);

    engine_log("Colls: %zu", room1ColMem->size());

    placeRoom(Direction::No, 0, 0);

    std::vector<double> rooms;

    for (int i = 0; i < TileManager::MAX_ROOMS; i++) {
        rooms.push_back((double)tileManager->world[i].x);
        rooms.push_back((double)tileManager->world[i].y);
    }

    delaunator::Delaunator d(rooms);

    for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
        printf("Triangle points: [[%f, %f], [%f, %f], [%f, %f]]\n",
               d.coords[2 * d.triangles[i]],         // tx0,
               d.coords[2 * d.triangles[i] + 1],     // ty0
               d.coords[2 * d.triangles[i + 1]],     // tx1
               d.coords[2 * d.triangles[i + 1] + 1], // ty1
               d.coords[2 * d.triangles[i + 2]],     // tx2
               d.coords[2 * d.triangles[i + 2] + 1]  // ty2
        );
    }
}
