#pragma once

#include "../Component.h"
#include "../Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>
#include <boost/hana.hpp>
#include <memory>

namespace hana = boost::hana;

struct TransformComponent : public Component
{
    TransformComponent(std::shared_ptr<Entity> owner)
    : Component(owner),
      localPosition(0.0f),
      localRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      localScale(1.0f),
      worldPosition(0.0f),
      worldRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      worldScale(1.0f) {}

    // Hana reflection
    BOOST_HANA_DEFINE_STRUCT(TransformComponent,
        (glm::vec3, localPosition),
        (glm::quat, localRotation),
        (glm::vec3, localScale),
        (glm::vec3, worldPosition),
        (glm::quat, worldRotation),
        (glm::vec3, worldScale)
    );

    // Local transform getters/setters
    void setLocalPosition(const glm::vec3 &pos)
    {
        localPosition = pos;
        updateWorldTransform();
    }

    void setLocalRotation(const glm::quat &rot)
    {
        localRotation = rot;
        updateWorldTransform();
    }

    void setLocalScale(const glm::vec3 &scale)
    {
        localScale = scale;
        updateWorldTransform();
    }

    const glm::vec3 &getLocalPosition() const { return localPosition; }
    const glm::quat &getLocalRotation() const { return localRotation; }
    const glm::vec3 &getLocalScale() const { return localScale; }

    // World transform getters/setters
    void setWorldPosition(const glm::vec3 &pos)
    {
        worldPosition = pos;
        updateLocalTransform();
    }

    void setWorldRotation(const glm::quat &rot)
    {
        worldRotation = rot;
        updateLocalTransform();
    }

    void setWorldScale(const glm::vec3 &scale)
    {
        worldScale = scale;
        updateLocalTransform();
    }

    const glm::vec3 &getWorldPosition() const { return worldPosition; }
    const glm::quat &getWorldRotation() const { return worldRotation; }
    const glm::vec3 &getWorldScale() const { return worldScale; }

    void setLocalMatrix(const glm::mat4 &localMatrix)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(localMatrix, localScale, localRotation, localPosition, skew, perspective);
        updateWorldTransform();
    }

    // Matrix calculations
    glm::mat4 getLocalMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), localPosition) *
               glm::mat4_cast(localRotation) *
               glm::scale(glm::mat4(1.0f), localScale);
    }

    glm::mat4 getWorldMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), worldPosition) *
               glm::mat4_cast(worldRotation) *
               glm::scale(glm::mat4(1.0f), worldScale);
    }

    void setWorldMatrix(const glm::mat4 &worldMatrix)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(worldMatrix, worldScale, worldRotation, worldPosition, skew, perspective);
        updateLocalTransform();
    }

    void updateWorldTransform();

    void updateLocalTransform();

    void updateLocalMatrix()
    {
        glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), localPosition) *
                               glm::mat4_cast(localRotation) *
                               glm::scale(glm::mat4(1.0f), localScale);
                               
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(localMatrix, localScale, localRotation, localPosition, skew, perspective);
        updateWorldTransform();
    }

    // Mantendo as funções antigas por compatibilidade
    void setPosition(glm::vec3 position) { setWorldPosition(position); }
    void setRotation(glm::quat rotation) { setWorldRotation(rotation); }
    void setScale(glm::vec3 scale) { setWorldScale(scale); }
};