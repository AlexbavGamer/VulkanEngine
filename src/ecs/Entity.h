#pragma once

#include <unordered_map>
#include <cstdint>
#include <typeindex>
#include <memory>
#include "Component.h"

using id_t = std::uint32_t;

class Entity {
    friend class Registry;
private:
    id_t id;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
public:
    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        components[std::type_index(typeid(T))] = component;
        return *component;
    }

    template<typename T>
    T& getComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        return *std::dynamic_pointer_cast<T>(components[std::type_index(typeid(T))]);
    }

    template<typename T>
    bool hasComponent() {
        return components.find(std::type_index(typeid(T))) != components.end();
    }
};
