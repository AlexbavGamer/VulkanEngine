#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "Entity.h"
#include <functional>

class Registry {
private:
    std::vector<std::shared_ptr<Entity>> entities;

public:
    id_t nextEntityId = 0;
    const std::vector<std::shared_ptr<Entity>>& getEntities() const {
        return entities;
    }
    
    std::shared_ptr<Entity> createEntity() 
    {
        auto entity = std::make_shared<Entity>();

        entity->id = nextEntityId++;

        entities.push_back(entity);

        return entity;
    }

    uint32_t size() const {
        return entities.size();
    }

    template<typename... Components, typename Func>
    void view(Func func) const {
        for (const auto& entity : entities) {
            if ((entity->template hasComponent<Components>() && ...)) {
                func(entity, entity->template getComponent<Components>()...);
            }
        }
    }
};
