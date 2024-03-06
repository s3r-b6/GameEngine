#pragma once

#include <memory>
#include <vector>

#include "SDL2/SDL_log.h"

#include "./types.h"

#include "./game_render.h"

#include "./globals.h"

using std::shared_ptr;
using std::vector;

// TODO: I'd like to use the bump allocators also here, but I have to think
// about how to handle it

struct EntityComponentBase {
    virtual ~EntityComponentBase() = default;
    virtual void update() {}
    virtual void render() {}
};

struct TransformComponent : EntityComponentBase {
    glm::vec2 pos, size;

    TransformComponent(glm::vec2 posIn, glm::vec2 sizeIn) { pos = posIn, size = sizeIn; }

    void setPos(glm::vec2 posIn) { pos = posIn; }
    void setSize(glm::vec2 sizeIn) { size = sizeIn; }
};

// AABB collisions
struct ColliderComponent : EntityComponentBase {
    shared_ptr<TransformComponent> transform;

    explicit ColliderComponent(shared_ptr<TransformComponent> t) { transform = t; }

    bool check_collision(glm::vec2 otherPos, glm::vec2 otherSize) {
        bool collX = transform->pos.x + transform->size.x >= otherPos.x &&
                     otherPos.x + otherSize.x >= transform->pos.x;
        bool collY = transform->pos.y + transform->size.y >= otherPos.y &&
                     otherPos.y + otherSize.y >= transform->pos.x;

        return collX && collY;
    }

    bool check_collision(TransformComponent *other) {
        bool collX = transform->pos.x + transform->size.x >= other->pos.x &&
                     other->pos.x + other->size.x >= transform->pos.x;
        bool collY = transform->pos.y + transform->size.y >= other->pos.y &&
                     other->pos.y + other->size.y >= transform->pos.x;

        return collX && collY;
    }
};

// TODO: Maybe look into a Messaging System
struct SpriteRenderer : EntityComponentBase {
    SpriteID sprite;
    shared_ptr<TransformComponent> transformComponent;

    RenderData *renderData;

    SpriteRenderer(RenderData *renderDataIn, SpriteID spriteIn, glm::vec2 sizeIn,
                   shared_ptr<TransformComponent> transform) {
        sprite = spriteIn;
        transformComponent = transform;

        renderData = renderDataIn;
    }

    void render() override {
        // SDL_Log("Trying to render at %f %f, spriteID %d, size %f %f", transformComponent->pos.x,
        // transformComponent->pos.y, sprite, size.x, size.y);
        drawSprite(renderData, sprite, transformComponent->pos, transformComponent->size);
    }
};

struct Entity {
    vector<shared_ptr<EntityComponentBase>> components;

    void render() {
        for (const auto &component : components) {
            component->render();
        }
    }

    void update() {
        for (const auto &component : components) {
            component->update();
        }
    }
};

// EntityManager class with a vector of shared pointers to Entity
struct EntityManager {
    vector<shared_ptr<Entity>> entities;

    void update() {
        for (const auto &entity : entities) {
            entity->update();
        }
    }

    void render() {
        for (const auto &entity : entities) {
            entity->render();
        }
    }
};
