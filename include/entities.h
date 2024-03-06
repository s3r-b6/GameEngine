#pragma once

#include <map>
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

struct BadComponent : EntityComponentBase {};

struct TransformComponent : EntityComponentBase {
    glm::vec2 pos, size;

    TransformComponent(glm::vec2 posIn, glm::vec2 sizeIn) { pos = posIn, size = sizeIn; }

    void setPos(glm::vec2 posIn) { pos = posIn; }
    void setSize(glm::vec2 sizeIn) { size = sizeIn; }
};

// AABB collisions
struct ColliderComponent : EntityComponentBase {
    shared_ptr<TransformComponent> transform;
    glm::vec2 size;

    ColliderComponent(shared_ptr<TransformComponent> t, glm::vec2 s) {
        transform = t;
        size = s;
    }

    bool checkCollisions(glm::vec2 otherPos, glm::vec2 otherSize) {
        bool collX =
            transform->pos.x + size.x >= otherPos.x && otherPos.x + otherSize.x >= transform->pos.x;
        bool collY =
            transform->pos.y + size.y >= otherPos.y && otherPos.y + otherSize.y >= transform->pos.y;

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

    template <typename T> shared_ptr<T> findComponent() {
        for (const auto &component : components) {
            shared_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(component);
            if (castedComponent) { return castedComponent; }
        }

        // Way too generic, might have to improve this
        SDL_Log("Entity failed to find a component");
        return nullptr;
    }

    void render() {
        for (const auto &component : components)
            component->render();
    }

    void update() {
        for (const auto &component : components)
            component->update();
    }
};

struct EntityManager {
    // Like a hashmap but allows for repeated keys
    std::multimap<char *, shared_ptr<Entity>> entities;

    void addEntity(char *name, shared_ptr<Entity> entity) {
        entities.insert(std::pair<char *, shared_ptr<Entity>>(name, entity));
    }

    shared_ptr<Entity> querySingleEntity(char *name) {
        auto it = entities.find(name);
        if (it != entities.end()) return it->second;

        SDL_Log("EntityManager failed to fint an entity '%s'", name);
        return nullptr;
    }

    std::vector<shared_ptr<Entity>> queryEntities(char *name) {
        vector<shared_ptr<Entity>> result;

        auto range = entities.equal_range(name);
        for (auto it = range.first; it != range.second; ++it)
            result.push_back(it->second);

        if (result.size() == 0)
            SDL_Log("EntityManager returned empty vector querying for entities '%s'", name);

        return result;
    }

    void update() {
        for (const auto &entity : entities) {
            entity.second->update();
        }
    }

    void render() {
        for (const auto &entity : entities) {
            entity.second->render();
        }
    }
};
