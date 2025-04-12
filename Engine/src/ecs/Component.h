#pragma once

#include <memory>

class Entity;

struct Component {
public:

    [[nodiscard]] Component(std::shared_ptr<Entity> owner = nullptr) 
        : owner(owner) {}

    virtual ~Component() = default;

    std::shared_ptr<Entity> getOwner() const { return owner; }

protected:
    std::shared_ptr<Entity> owner;
};