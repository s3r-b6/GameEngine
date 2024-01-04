#include "game.h"
#include "assets.h"
#include "renderer.h"

void update_game() {
    draw_sprite(gRenderData, Chest, {200.f, 200.f}, {100.f, 100.f});
}
