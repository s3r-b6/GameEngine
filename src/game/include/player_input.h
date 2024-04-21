#pragma once

#include "./types.h"

#include "./entities.h"
#include "./game_global.h"
#include "./input.h"
#include "./tiles.h"

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
    if (input->keyIsDown('w')) {
        newPos->y -= playerSpeed;
        spriteRenderer->animatedSprite = PlayerU_Walk;
        moved = true;
    } else if (input->keyIsDown('s')) {
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

    if (input->keyIsDown('d')) {
        newPos->x += playerSpeed;
        spriteRenderer->animatedSprite = PlayerL_Walk;
        moved = true;
    } else if (input->keyIsDown('a')) {
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
