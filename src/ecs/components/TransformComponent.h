#pragma once

#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

struct TransformComponent : public Component
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 getMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), position) *
               glm::yawPitchRoll(rotation.x, rotation.y, rotation.z) *
               glm::scale(glm::mat4(1.0f), scale);
    }

    void setPosition(glm::vec3 position)
    {
        this->position = position;
    }

    void setRotation(glm::vec3 rotation)
    {
        this->rotation = rotation;
    }

    void setScale(glm::vec3 scale)
    {
        this->scale = scale;
    }

    void renderComponent() override
    {
        //TODO: Implement rendering of the transform component (ImGui)
    }
};