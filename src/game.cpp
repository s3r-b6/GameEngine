#include "game.h"
#include "renderer.h"

EXPORT_FN void updateGame(RenderData *renderDataIn) {
    gRenderData = renderDataIn;

    glm::ivec2 pos = {10.f, 10.f};

    draw_sprite(gRenderData, Chest, {100.f, 0.f}, {100.f, 100.f});
    draw_sprite(gRenderData, Player, {300.f, 000.f}, {100.f, 100.f});
}
