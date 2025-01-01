#pragma once

#include <memory>

class Entity;

struct Component {
public:

    Component(std::shared_ptr<Entity> owner = nullptr) 
        : owner(owner) {}

    virtual ~Component() = default;
    virtual void renderComponent() { }

protected:
    std::shared_ptr<Entity> owner;
};