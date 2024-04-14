#include "game_input.h"
#include "entities.h"
#include "game_global.h"
#include "tiles.h"

// TODO: I don't like this
bool checkTileCollisions(ColliderComponent *collider) {
    for (auto coll : gameState->tileManager->collisions) {
        if (collider->checkCollisions({coll.worldX * TILESIZE, coll.worldY * TILESIZE}, {16, 16}))
            return true;
    }

    return false;
}

auto move = [](u32 player_id) {
    float playerSpeed = 2.f;

    auto player = gameState->entityManager->entities[player_id];
    auto transform = player->findComponent<TransformComponent>();
    auto spriteRenderer = player->findComponent<AnimatedSpriteRenderer>();
    auto collider = player->findComponent<ColliderComponent>();

    auto oldPos = transform->pos;
    auto newPos = &transform->pos;

    bool moved = false;
    if (engine_input->keyIsDown('w')) {
        newPos->y -= playerSpeed;
        spriteRenderer->animatedSprite = PlayerU_Walk;
        moved = true;
    } else if (engine_input->keyIsDown('s')) {
        newPos->y += playerSpeed;
        spriteRenderer->animatedSprite = PlayerD_Walk;
        moved = true;
    }

    // TODO: This is the dumbest way to allow for movement when it is not possible in 1
    // direction. this is horrible. also: this might be too naive (maybe somehow the player
    // can end stuck forever inside a collision?)
    if (moved && checkTileCollisions(collider)) {
        transform->pos = oldPos;
        moved = false;
    }

    oldPos = transform->pos;

    if (engine_input->keyIsDown('d')) {
        newPos->x += playerSpeed;
        spriteRenderer->animatedSprite = PlayerL_Walk;
        moved = true;
    } else if (engine_input->keyIsDown('a')) {
        newPos->x -= playerSpeed;
        spriteRenderer->animatedSprite = PlayerR_Walk;
        moved = true;
    }

    if (moved && checkTileCollisions(collider)) {
        transform->pos = oldPos;
        moved = false;
    }

    spriteRenderer->animating = moved;
};
auto tileActions = [](u32) {
    if (engine_input->keyIsDown('=')) {
        engine_log("Saving world state");
        gameState->tileManager->serialize();
    } else if (engine_input->keyIsDown('9')) {
        engine_log("Reloading world");
        gameState->tileManager->deserialize();
    } else if (engine_input->keyIsDown('-')) {
        engine_log("Clearing tile manager");
        gameState->tileManager->clear();
    }

    if (engine_input->keyIsDown('b')) {
        engine_log("Back layer");
        // selectedWorldLayer = 0;
    } else if (engine_input->keyIsDown('f')) {
        engine_log("Front layer");
        // selectedWorldLayer = 1;
    }
};

void setup_keys() {
    inputManager->registerFunction("TilePickerToggle", InputManager::GAME_MODE, '1', [](u32) {
        g->gameState->tileManager->tilePickerShown = !g->gameState->tileManager->tilePickerShown;
    });
    inputManager->registerFunction("Movement", InputManager::GAME_MODE, '\0', move);
    inputManager->registerFunction("TileActions", InputManager::GAME_MODE, '\0', tileActions);
}