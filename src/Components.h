#pragma once
#include "ecs/components/CameraComponent.h"
namespace camera {
    void updateCamera(CameraComponent& camera, const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
}