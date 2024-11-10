#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "Entity.h"
#include <functional>

class Registry
{
private:
    std::vector<std::shared_ptr<Entity>> entities;
    id_t nextEntityId = 0;

public:
    Entity& createEntity()
    {
        auto entity = std::make_shared<Entity>();
        entity->id = nextEntityId++;
        entities.push_back(entity);
        return *entity;
    }

    template<typename... Components, typename Func>
    void view(Func func) {
        for (const auto& entity : entities) {
            if ((entity->hasComponent<Components>() && ...)) {
                func(*entity, entity->getComponent<Components>()...);
            }
        }
    }
};