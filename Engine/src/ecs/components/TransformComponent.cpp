#include "TransformComponent.h"

void TransformComponent::updateWorldTransform()
{
    glm::mat4 parentWorldMatrix(1.0f);

    if (owner)
    {
        if (auto parent = owner->getParent())
        {
            if (parent->template hasComponent<TransformComponent>())
            {
                parentWorldMatrix = parent->template getComponent<TransformComponent>().getWorldMatrix();
            }
        }
    }

    glm::mat4 worldMatrix = parentWorldMatrix * getLocalMatrix();

    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(worldMatrix, worldScale, worldRotation, worldPosition, skew, perspective);
}

void TransformComponent::updateLocalTransform()
{
    glm::mat4 parentWorldMatrix(1.0f);

    if (owner)
    { // Não precisa mais do lock() pois já é shared_ptr
        if (auto parent = owner->getParent())
        {
            if (parent->template hasComponent<TransformComponent>())
            {
                parentWorldMatrix = parent->template getComponent<TransformComponent>().getWorldMatrix();
            }
        }
    }

    glm::mat4 localMatrix = glm::inverse(parentWorldMatrix) * getWorldMatrix();

    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(localMatrix, localScale, localRotation, localPosition, skew, perspective);
}