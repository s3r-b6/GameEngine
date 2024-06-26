#pragma once

#include "./types.h"

#include "./input.h"
#include "./tiles.h"

void playerActions() {
    float playerSpeed = 2.f;

    local_persist u32 p_id = MAX_U32;
    local_persist Entity *player = nullptr;
    local_persist TransformComponent *transform;
    local_persist AnimatedSpriteRenderer *spriteRenderer;
    local_persist ColliderComponent *collider;

    if (p_id != gameState->player_id) {
        p_id = gameState->player_id;
        player = gameState->entityManager->entities[gameState->player_id];
        transform = player->findComponent<TransformComponent>();
        spriteRenderer = player->findComponent<AnimatedSpriteRenderer>();
        collider = player->findComponent<ColliderComponent>();
    }

    local_persist Direction direction = D;

    if (!player) {
        engine_log("handlePlayerMovement() could not find a player!");
        return;
    }

    if (input->keyJustPressed('v')) { renderData->vsync = !renderData->vsync; }
    if (input->keyJustPressed('n')) { renderData->palletize = !renderData->palletize; }

    if (input->keyJustPressed('=')) {
        renderData->gameCamera.dimensions /= 2;
    } else if (input->keyJustPressed('-')) {
        renderData->gameCamera.dimensions *= 2;
    }

    bool moved = false;
    auto oldPos = transform->pos, *newPos = &transform->pos;
    if (input->keyIsDown('w')) {
        newPos->y -= playerSpeed;
        spriteRenderer->animatedSprite = PlayerU_Walk;
        moved = true;
        direction = U;
    } else if (input->keyIsDown('s')) {
        newPos->y += playerSpeed;
        spriteRenderer->animatedSprite = PlayerD_Walk;
        moved = true;
        direction = D;
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
        direction = R;
    } else if (input->keyIsDown('a')) {
        newPos->x -= playerSpeed;
        spriteRenderer->animatedSprite = PlayerR_Walk;
        moved = true;
        direction = L;
    }

    if (moved && checkTileCollisions(collider)) {
        transform->pos = oldPos;
        moved = false;
    }

    renderData->gameCamera.pos = lerp(renderData->gameCamera.pos, transform->pos, 0.12);

    spriteRenderer->animating = moved;

    // TODO: Would be cool to have a renderOnce() or something like that, instead of looping
    if (input->lMouseDown()) {
        switch (direction) {
        case U:  spriteRenderer->animatedSprite = PlayerU_Attack; break;
        case L:  spriteRenderer->animatedSprite = PlayerL_Attack; break;
        case R:  spriteRenderer->animatedSprite = PlayerR_Attack; break;
        default: spriteRenderer->animatedSprite = PlayerD_Attack; break;
        }

        spriteRenderer->animating = true;
    } else {
        switch (direction) {
        case U:  spriteRenderer->idleSprite = PlayerU; break;
        case L:  spriteRenderer->idleSprite = PlayerL; break;
        case R:  spriteRenderer->idleSprite = PlayerR; break;
        default: spriteRenderer->idleSprite = PlayerD; break;
        }
    }
}
