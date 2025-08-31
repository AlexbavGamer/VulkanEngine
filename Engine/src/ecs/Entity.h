#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <typeindex>
#include <stdexcept>
#include <cstdint>
#include "Component.h"

using id_t = std::uint32_t;

class Entity : public std::enable_shared_from_this<Entity>
{
    friend class Registry;

private:
    id_t id;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    std::string name;
    std::weak_ptr<Entity> parent;                  // Referência ao pai
    std::vector<std::shared_ptr<Entity>> children; // Lista de filhos

public:
    Entity()
    {
        components = std::unordered_map<std::type_index, std::shared_ptr<Component>>();
    }

    id_t getId() const { return id; }

    std::string getName() const { return name; }
    void setName(const std::string &newName) { name = newName; }
    // --- Hierarquia ---

    // Define o pai desta entidade
    void setParent(const std::shared_ptr<Entity> &newParent)
    {
        if (auto currentParent = parent.lock())
        {
            currentParent->removeChild(shared_from_this());
        }

        parent = newParent;

        if (newParent)
        {
            newParent->addChild(shared_from_this());
        }
    }

    // Obtém o pai
    std::shared_ptr<Entity> getParent() const
    {
        return parent.lock();
    }

    // Adiciona um filho
    void addChild(const std::shared_ptr<Entity> &child)
    {
        children.push_back(child);
        child->parent = shared_from_this();
        updateTransformHierarchy();
    }

    // Remove um filho
    void removeChild(const std::shared_ptr<Entity> &child)
    {
        children.erase(
            std::remove(children.begin(), children.end(), child),
            children.end());
        child->parent.reset();
    }

    // Obtém todos os filhos
    const std::vector<std::shared_ptr<Entity>> &getChildren() const
    {
        return children;
    }

    // Verifica se é a raiz (sem pai)
    bool isRoot() const
    {
        return parent.expired();
    }

    // Verifica se é folha (sem filhos)
    bool isLeaf() const
    {
        return children.empty();
    }

    // --- Componentes (mantidos como antes) ---

    template <typename T, typename... Args>
    T &addComponent(Args &&...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto component = std::make_shared<T>(shared_from_this(), std::forward<Args>(args)...);
        components[std::type_index(typeid(T))] = component;
        return *component;
    }

    template <typename T, typename... Args>
    T &AddOrGetComponent(Args &&...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end())
        {
            return *std::dynamic_pointer_cast<T>(it->second);
        }
        return addComponent<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    T &getComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto component = std::dynamic_pointer_cast<T>(components[std::type_index(typeid(T))]);
        if (!component)
        {
            throw std::runtime_error("Component not found or incorrect type");
        }
        return *component;
    }

    const std::unordered_map<std::type_index, std::shared_ptr<Component>> &getComponents() const
    {
        return components;
    }

    template <typename T>
    bool hasComponent() const
    {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    void updateTransformHierarchy();
};
