#pragma once

#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>

struct TransformComponent : public Component
{
    TransformComponent(std::shared_ptr<Entity> owner)
        : Component(owner) {}
    
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
        if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Adicionar espaçamento entre os headers e os controles
            ImGui::Indent(15.0f); // Espaco à esquerda para se alinhar

            // Adicionar padding lateral e também ajustar a altura do cabeçalho
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f)); // Padding lateral e vertical

            // Seção Position
            if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginChild("PositionChild", ImVec2(0, 100), true))
                {
                    // Adicionar espaçamento vertical entre os controles
                    ImGui::PushItemWidth(-1); // Usar a largura disponível
                    ImGui::DragFloat("X", &position.x, 0.1f);
                    ImGui::DragFloat("Y", &position.y, 0.1f);
                    ImGui::DragFloat("Z", &position.z, 0.1f);
                    ImGui::PopItemWidth();
                }
                ImGui::EndChild();
            }

            // Seção Rotation
            if (ImGui::CollapsingHeader("Rotation", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginChild("RotationChild", ImVec2(0, 100), true))
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::DragFloat("X", &rotation.x, 0.1f);
                    ImGui::DragFloat("Y", &rotation.y, 0.1f);
                    ImGui::DragFloat("Z", &rotation.z, 0.1f);
                    ImGui::PopItemWidth();
                }
                ImGui::EndChild();
            }

            // Seção Scale
            if (ImGui::CollapsingHeader("Scale", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginChild("ScaleChild", ImVec2(0, 100), true))
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::DragFloat("X", &scale.x, 0.1f);
                    ImGui::DragFloat("Y", &scale.y, 0.1f);
                    ImGui::DragFloat("Z", &scale.z, 0.1f);
                    ImGui::PopItemWidth();
                }
                ImGui::EndChild();
            }

            // Restaurar padding e alinhamento
            ImGui::PopStyleVar();
            ImGui::Unindent(15.0f); // Remover o recuo à esquerda
        }
    }
};