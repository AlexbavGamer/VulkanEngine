#pragma once

#include <unordered_map>
#include <cstdint>
#include <typeindex>
#include <stdexcept>
#include <memory>
#include "Component.h"

using id_t = std::uint32_t;

class Entity {
    friend class Registry;
private:
    id_t id;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
public:
    Entity() {
        components = std::unordered_map<std::type_index, std::shared_ptr<Component>>();
    }

    id_t getId() const { return id; }

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        
        components[std::type_index(typeid(T))] = component;
        
        if (components.find(std::type_index(typeid(T))) == components.end()) {
            throw std::runtime_error("Falha ao adicionar o componente ao map de componentes.");
        }
        
        return *component;
    }

    template<typename T, typename... Args>
    T& AddOrGetComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return *std::dynamic_pointer_cast<T>(it->second);
        }
        return addComponent<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    T& getComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto component = std::dynamic_pointer_cast<T>(components[std::type_index(typeid(T))]);
        if (!component) {
            throw std::runtime_error("Component not found or incorrect type");
        }
        return *component;
    }

    const std::unordered_map<std::type_index, std::shared_ptr<Component>>& getComponents() const {
        return components;
    }

    template<typename T>
    bool hasComponent() const {
        return components.find(std::type_index(typeid(T))) != components.end();
    }
};
