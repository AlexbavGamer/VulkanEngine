#pragma once
#include "../Component.h"

struct GizmoComponent : Component {
    GizmoComponent(std::shared_ptr<Entity> owner) : Component(owner) {}
};