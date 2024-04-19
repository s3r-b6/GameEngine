#include "./entities.h"
#include "./game_global.h"
#include "./input.h"
#include "./tiles.h"

// TODO: I don't like this
bool checkTileCollisions(ColliderComponent *collider) {
    for (auto coll : gameState->tileManager->collisions) {
        if (collider->checkCollisions({coll.worldX * TILESIZE, coll.worldY * TILESIZE}, {16, 16}))
            return true;
    }

    return false;
}

void handlePlayerMovement(u32 player_id) {
    float playerSpeed = 2.f;

    local_persist u32 prev_player_id = 0;

    if (prev_player_id == 0) { prev_player_id = player_id; }

    local_persist auto player = gameState->entityManager->entities[player_id];
    local_persist auto transform = player->findComponent<TransformComponent>();
    local_persist auto spriteRenderer = player->findComponent<AnimatedSpriteRenderer>();
    local_persist auto collider = player->findComponent<ColliderComponent>();

    if (prev_player_id != player_id) {
        player = gameState->entityManager->entities[player_id];
        transform = player->findComponent<TransformComponent>();
        spriteRenderer = player->findComponent<AnimatedSpriteRenderer>();
        collider = player->findComponent<ColliderComponent>();
    }

    if (!player) {
        engine_log("handlePlayerMovement() could not find a player!");
        return;
    }

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
}

void handletileActions() {
    if (engine_input->keyJustPressed('=')) {
        engine_log("Saving world state");
        gameState->tileManager->serialize();
    } else if (engine_input->keyJustPressed('9')) {
        engine_log("Reloading world");
        gameState->tileManager->deserialize();
    } else if (engine_input->keyJustPressed('-')) {
        engine_log("Clearing tile manager");
        gameState->tileManager->clear();
    }

    if (engine_input->keyJustPressed('b')) {
        engine_log("Back layer");
        // selectedWorldLayer = 0;
    } else if (engine_input->keyJustPressed('f')) {
        engine_log("Front layer");
        // selectedWorldLayer = 1;
    }
}

void toggleTilePicker() {
    g->gameState->tileManager->tilePickerShown = !g->gameState->tileManager->tilePickerShown;
}

void inputFunctions(u32 player_id) {
    if (g->input->keyJustPressed('1')) { toggleTilePicker(); }
    handlePlayerMovement(player_id);
    handletileActions();
}
