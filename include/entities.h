#pragma once

#include <map>
#include <memory>
#include <stack>
#include <vector>

#include "SDL2/SDL_log.h"

#include "./types.h"

#include "./game_render.h"

#include "./globals.h"
#include "assets.h"
#include "engine_lib.h"

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
    TransformComponent *transform;
    glm::vec2 size;

    ColliderComponent(TransformComponent *t, glm::vec2 s) {
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

struct AnimatedSpriteRenderer : EntityComponentBase {
    AnimatedSpriteID sprite;
    TransformComponent *transformComponent;
    int fps, maxFrames;
    float *deltaTime;

    RenderData *renderData;

    AnimatedSpriteRenderer(RenderData *renderDataIn, AnimatedSpriteID spriteIn, glm::vec2 sizeIn,
                           TransformComponent *transform, int fpsIn, float *dt, int framesIn) {
        sprite = spriteIn;
        transformComponent = transform;
        renderData = renderDataIn;
        fps = fpsIn;
        deltaTime = dt;
        maxFrames = framesIn;
    }

    void render() override {
        local_persist float timer;
        local_persist int curr_frame;
        if (!timer) { timer = fps / TARGET_FPS; }
        if (!curr_frame) { curr_frame = 0; }
        timer -= *deltaTime;

        if (timer <= 0) {
            if (++curr_frame >= maxFrames) curr_frame = 0;
            timer = fps / TARGET_FPS;
        }

        drawAnimatedSprite(renderData, sprite, transformComponent->pos, transformComponent->size,
                           curr_frame);
    }
};

struct SpriteRenderer : EntityComponentBase {
    SpriteID sprite;
    TransformComponent *transformComponent;

    RenderData *renderData;

    SpriteRenderer(RenderData *renderDataIn, SpriteID spriteIn, glm::vec2 sizeIn,
                   TransformComponent *transform) {
        sprite = spriteIn;
        transformComponent = transform;
        renderData = renderDataIn;
    }

    void render() override {
        drawSprite(renderData, sprite, transformComponent->pos, transformComponent->size);
    }
};

struct Entity {
    bool initialized = false;
    vector<EntityComponentBase *> components;

    bool clear() {
        if (!initialized) { crash("Tried to clear an uninitialized component"); }

        for (auto c : components) {
            free(c);
        }

        components.clear();
        initialized = false;

        return true;
    }

    template <typename T> T *findComponent() {
        for (const auto &component : components) {
            T *castedComponent = dynamic_cast<T *>(component);
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

// This is kind of cool, but I am not sure how to handle reutilizing entities
struct EntityManager {
    std::vector<Entity *> entities;
    std::stack<u32> freeEntities;

    void init(u32 poolsize) {
        entities = std::vector<Entity *>();
        freeEntities = std::stack<u32>();

        for (u32 i = 0; i < poolsize; i++) {
            entities.push_back(new Entity());
            freeEntities.push(i);
        }
    }

    // This assumes that when you ask for an ID it is going to be initialized
    // right then. This should be OK
    u32 getUninitializedID() {
        if (freeEntities.size() == 0) {
            crash("Could not find a free entity");
            return 0;
        }

        u32 id = freeEntities.top();
        entities[id]->initialized = true;
        freeEntities.pop();
        return id;
    }

    // Might fail if the id is not valid...
    bool freeEntity(u32 id) {
        entities[id]->clear();
        freeEntities.push(id);
        return true;
    }

    // This is slower than freeing by ID, but might be useful?...
    bool freeEntity(Entity *entity_ptr) {
        int max = entities.size();
        for (u32 i = 0; i < max; i++) {
            Entity *original_ptr = entities[i];
            if (original_ptr == entity_ptr) {
                original_ptr->clear();
                freeEntities.push(i);
                return true;
            }
        }

        crash("Could not find the entity to be cleared by address");
        return false;
    }

    void update() {
        for (auto &entity : entities) {
            if (!entity->initialized) continue;
            entity->update();
        }
    }

    void render() {
        for (auto &entity : entities) {
            if (!entity->initialized) continue;
            entity->render();
        }
    }
};
