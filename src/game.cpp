#include "game.h"
#include "renderer.h"

EXPORT_FN void updateGame(RenderData *renderDataIn) {
    gRenderData = renderDataIn;

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            draw_sprite(gRenderData, Player, {100.f * x, 200.f * y},
                        {100.f, 100.f});
            draw_sprite(gRenderData, Chest, {150.f * x, 100.f * y},
                        {100.f, 100.f});
        }
    }
}
