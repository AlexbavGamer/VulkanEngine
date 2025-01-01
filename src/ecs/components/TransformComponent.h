#pragma once

#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>
#include <memory>

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

    // Local transforms
    glm::vec3 localPosition;
    glm::quat localRotation;
    glm::vec3 localScale;

    // World transforms
    glm::vec3 worldPosition;
    glm::quat worldRotation;
    glm::vec3 worldScale;

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

    void updateWorldTransform()
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

    void updateLocalTransform()
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

    // Mantendo as funções antigas por compatibilidade
    void setPosition(glm::vec3 position) { setWorldPosition(position); }
    void setRotation(glm::quat rotation) { setWorldRotation(rotation); }
    void setScale(glm::vec3 scale) { setWorldScale(scale); }

    void renderComponent() override
    {
        if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent(15.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));

            // Local Transform
            if (ImGui::CollapsingHeader("Local Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginChild("LocalTransformChild", ImVec2(0, 300), true))
                {
                    // Position
                    if (ImGui::TreeNode("Position"))
                    {
                        ImGui::PushItemWidth(-1);
                        bool changed = false;
                        changed |= ImGui::DragFloat("X##Local", &localPosition.x, 0.1f);
                        changed |= ImGui::DragFloat("Y##Local", &localPosition.y, 0.1f);
                        changed |= ImGui::DragFloat("Z##Local", &localPosition.z, 0.1f);
                        if (changed)
                            updateWorldTransform();
                        ImGui::PopItemWidth();
                        ImGui::TreePop();
                    }

                    // Rotation (convertendo quaternion para euler angles para visualização)
                    if (ImGui::TreeNode("Rotation"))
                    {
                        ImGui::PushItemWidth(-1);
                        glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(localRotation));
                        bool changed = false;
                        changed |= ImGui::DragFloat("X##LocalRot", &eulerAngles.x, 1.0f);
                        changed |= ImGui::DragFloat("Y##LocalRot", &eulerAngles.y, 1.0f);
                        changed |= ImGui::DragFloat("Z##LocalRot", &eulerAngles.z, 1.0f);
                        if (changed)
                        {
                            glm::vec3 radians = glm::radians(eulerAngles);
                            localRotation = glm::quat(radians);
                            updateWorldTransform();
                        }
                        ImGui::PopItemWidth();
                        ImGui::TreePop();
                    }

                    // Scale
                    if (ImGui::TreeNode("Scale"))
                    {
                        ImGui::PushItemWidth(-1);
                        bool changed = false;
                        changed |= ImGui::DragFloat("X##LocalScale", &localScale.x, 0.1f);
                        changed |= ImGui::DragFloat("Y##LocalScale", &localScale.y, 0.1f);
                        changed |= ImGui::DragFloat("Z##LocalScale", &localScale.z, 0.1f);
                        if (changed)
                            updateWorldTransform();
                        ImGui::PopItemWidth();
                        ImGui::TreePop();
                    }
                }
                ImGui::EndChild();
            }

            // World Transform (Read-only display)
            if (ImGui::CollapsingHeader("World Transform"))
            {
                if (ImGui::BeginChild("WorldTransformChild", ImVec2(0, 300), true))
                {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Cor mais escura para indicar read-only

                    ImGui::Text("Position");
                    ImGui::PushItemWidth(-1);
                    ImGui::DragFloat("X##World", (float *)&worldPosition.x, 0.1f, 0.0f, 0.0f, "%.3f");
                    ImGui::DragFloat("Y##World", (float *)&worldPosition.y, 0.1f, 0.0f, 0.0f, "%.3f");
                    ImGui::DragFloat("Z##World", (float *)&worldPosition.z, 0.1f, 0.0f, 0.0f, "%.3f");

                    ImGui::Text("Scale");
                    ImGui::DragFloat("X##WorldScale", (float *)&worldScale.x, 0.1f, 0.0f, 0.0f, "%.3f");
                    ImGui::DragFloat("Y##WorldScale", (float *)&worldScale.y, 0.1f, 0.0f, 0.0f, "%.3f");
                    ImGui::DragFloat("Z##WorldScale", (float *)&worldScale.z, 0.1f, 0.0f, 0.0f, "%.3f");
                    ImGui::PopItemWidth();

                    ImGui::PopStyleColor();
                }
                ImGui::EndChild();
            }

            ImGui::PopStyleVar();
            ImGui::Unindent(15.0f);
        }
    }
};