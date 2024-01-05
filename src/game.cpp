#include "game.h"
#include "renderer.h"

EXPORT_FN void updateGame(RenderData *renderDataIn) {
    gRenderData = renderDataIn;

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            SpriteID sp = x + y > 5 ? Player : Chest;

            draw_sprite(gRenderData, sp, {100.f * x, 100.f * y},
                        {100.f, 100.f});
        }
    }
}
