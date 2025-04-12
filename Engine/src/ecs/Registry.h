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

public:
    id_t nextEntityId = 0;
    const std::vector<std::shared_ptr<Entity>> &getEntities() const
    {
        return entities;
    }

    void addEntity(std::shared_ptr<Entity> entity)
    {
        entity->id = nextEntityId++;
        
        entities.push_back(entity);
    }

    template <typename... TArgs>
    std::shared_ptr<Entity> createEntity(TArgs&&... args)
    {
        auto entity = std::make_shared<Entity>(std::forward<TArgs>(args)...);

        entity->id = nextEntityId++;

        entities.push_back(entity);

        return entity;
    }

    void removeEntity(std::shared_ptr<Entity> entity)
    {
        auto it = std::remove(entities.begin(), entities.end(), entity);
        if (it != entities.end())
        {
            entities.erase(it, entities.end());
        }
    }

    uint32_t size() const
    {
        return entities.size();
    }

    template <typename... Components>
    std::vector<std::shared_ptr<Entity>> viewWithSpecificComponents() const
    {
        std::vector<std::shared_ptr<Entity>> result;

        for (const auto &entity : entities)
        {
            if ((entity->template hasComponent<Components>() && ...))
            {
                result.push_back(entity);
            }
        }

        return result;
    }

    template <typename... Components, typename Func>
    void view(Func func) const
    {
        for (const auto &entity : entities)
        {
            if ((entity->template hasComponent<Components>() && ...))
            {
                func(entity, entity->template getComponent<Components>()...);
            }
        }
    }
};
