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
    glm::vec2 pos;

    explicit TransformComponent(glm::vec2 posIn) { pos = posIn; }

    void setPos(float x, float y) {
        pos = {x, y};
        // SDL_Log("New pos: %f %f", pos.x, pos.y);
    }
};

// TODO: Maybe look into a Messaging System
struct SpriteRenderer : EntityComponentBase {
    SpriteID sprite;
    shared_ptr<TransformComponent> transformComponent;

    RenderData *renderData;

    glm::vec2 size;

    SpriteRenderer(RenderData *renderDataIn, SpriteID spriteIn, glm::vec2 sizeIn,
                   shared_ptr<TransformComponent> transform) {
        sprite = spriteIn;
        transformComponent = transform;

        renderData = renderDataIn;

        size = sizeIn;
    }

    void render() override {
        // SDL_Log("Trying to render at %f %f, spriteID %d, size %f %f",
        //         transformComponent->pos.x, transformComponent->pos.y, sprite,
        //         size.x, size.y);
        drawSprite(renderData, sprite, transformComponent->pos, size);
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
