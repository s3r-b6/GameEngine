#pragma once

#include <typeinfo>

#include <stack>
#include <vector>

#include "./engine_global.h"

#include "./game.h"
#include "./game_render.h"

struct EntityComponentBase {
    u32 entityID;

    virtual ~EntityComponentBase() = default;
    virtual void update() {}
    virtual void render() {}
};

struct Entity {
    bool initialized = false;
    std::vector<EntityComponentBase *> components;

    bool clear() {
        if (!initialized) { crash("Tried to clear an uninitialized component"); }

        for (auto &c : components) { free(c); }

        components.clear();
        initialized = false;

        return true;
    }

    template <typename T> T *findComponent() {
        for (auto &component : components) {
            T *castedComponent = dynamic_cast<T *>(component);
            if (castedComponent) { return castedComponent; }
        }

        engine_log("Entity failed to find a component %s", typeid(T).name());

        return nullptr;
    }

    void render() {
        for (const auto &component : components) {
            if (component) component->render();
        }
    }

    void update() {
        for (const auto &component : components) {
            if (component) component->update();
        }
    }
};

// TODO: store a static hashmap in the engine memory to store ids after hot reload
struct EntityManager {
    std::vector<Entity *> entities;
    std::stack<u32> freeEntities;

    EntityManager(u32 poolsize, BumpAllocator *allocator) {
        entities = std::vector<Entity *>();
        freeEntities = std::stack<u32>();

        entities.reserve(poolsize);

        for (u32 i = 0; i < poolsize; i++) {
            Entity *e = new (allocator->alloc(sizeof(Entity))) Entity();
            entities.push_back(e);
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
        engine_log("Popped free_entity: %u", id);
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

struct TransformComponent : EntityComponentBase {
    glm::vec2 pos, size;
    TransformComponent(glm::vec2 posIn, glm::vec2 sizeIn) { pos = posIn, size = sizeIn; }
};

// AABB collisions
struct ColliderComponent : EntityComponentBase {
    TransformComponent *transform;
    glm::vec2 size;

    ColliderComponent(u32 player_id, glm::vec2 s) {
        entityID = player_id;

        auto e = gameState->entityManager->entities[entityID];
        // engine_log("ID: %u || %zu", entityID, e->components.size());
        transform = e->findComponent<TransformComponent>();

        size = s;
    }

    bool checkCollisions(glm::vec2 otherPos, glm::vec2 otherSize) {
        glm::vec2 halfSize = size * 0.5f;
        glm::vec2 otherHalfSize = otherSize * 0.5f;

        glm::vec2 center = transform->pos + halfSize;
        glm::vec2 otherCenter = otherPos + otherHalfSize;

        float deltaX = center.x - otherCenter.x;
        float deltaY = center.y - otherCenter.y;

        float minDistanceX = halfSize.x + otherHalfSize.x;
        float minDistanceY = halfSize.y + otherHalfSize.y;

        if (std::abs(deltaX) >= minDistanceX || std::abs(deltaY) >= minDistanceY) { return false; }
        return true;
    }
};

struct AnimatedSpriteRenderer : EntityComponentBase {
    RenderData *renderData;
    bool animating = true;

    AnimatedSpriteID animatedSprite;
    SpriteID idleSprite = INVALID;

    int spritesPerSecond, maxFrames, currFrame;
    double *deltaTime, timer;

    TransformComponent *transform;

    AnimatedSpriteRenderer(u32 id, RenderData *renderDataIn, AnimatedSpriteID spriteIn, int fpsIn,
                           double *dt, int framesIn, SpriteID idleSpriteIn) {
        entityID = id;

        auto e = gameState->entityManager->entities[entityID];
        transform = e->findComponent<TransformComponent>();

        renderData = renderDataIn;

        spritesPerSecond = fpsIn;
        maxFrames = framesIn;
        animatedSprite = spriteIn;
        idleSprite = idleSpriteIn;

        deltaTime = dt; // This is a reference to the float* dt global

        timer = spritesPerSecond / TARGET_FPS;
        currFrame = 0;
    }

    void render() override {
        if (animating) {
            timer -= *deltaTime;
            if (timer <= 0) {
                if (++currFrame >= maxFrames) { currFrame = 0; }
                timer = spritesPerSecond / TARGET_FPS;
            }

            drawAnimatedSprite(animatedSprite, transform->pos, transform->size, currFrame);
        } else if (idleSprite != INVALID) {
            drawSprite(idleSprite, transform->pos, transform->size);
        } else {
            crash("AnimatedSpriteRenderer does not know what to draw");
        }
    }
};

struct SpriteRenderer : EntityComponentBase {
    SpriteID sprite;
    RenderData *renderData;
    TransformComponent *transform;

    SpriteRenderer(u32 id, RenderData *renderDataIn, SpriteID spriteIn) {
        entityID = id;
        renderData = renderDataIn;
        auto e = gameState->entityManager->entities[entityID];
        engine_log("ID: %u || %zu", entityID, e->components.size());
        transform = e->findComponent<TransformComponent>();
    }

    void render() override { drawSprite(sprite, transform->pos, transform->size); }
};
