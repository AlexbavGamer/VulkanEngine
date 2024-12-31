#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include "imgui.h"

struct LightComponent : public Component
{
    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

    LightType type = LightType::Point;
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float range = 10.0f;

    // For spot lights
    float innerCutoff = glm::cos(glm::radians(12.5f));
    float outerCutoff = glm::cos(glm::radians(17.5f));

    // For directional lights
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

    // For point and spot lights
    glm::vec3 position = glm::vec3(0.0f);

    // Attenuation factors
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    // Set light position (for point and spot lights)
    void setPosition(const glm::vec3 &newPosition)
    {
        position = newPosition;
    }

    // Set light direction (for directional and spot lights)
    void setDirection(const glm::vec3 &newDirection)
    {
        direction = glm::normalize(newDirection);
    }

    // Update the light's color
    void setColor(const glm::vec3 &newColor)
    {
        color = newColor;
    }

    // Update the light's intensity
    void setIntensity(float newIntensity)
    {
        intensity = newIntensity;
    }

    // Update the light's range (only relevant for point lights)
    void setRange(float newRange)
    {
        range = newRange;
    }

    // Set the cutoff angles for spotlights
    void setSpotlightCutoffs(float inner, float outer)
    {
        innerCutoff = glm::cos(glm::radians(inner));
        outerCutoff = glm::cos(glm::radians(outer));
    }

    // Set attenuation factors
    void setAttenuation(float c, float l, float q)
    {
        constant = c;
        linear = l;
        quadratic = q;
    }

    void setType(LightType newType)
    {
        type = newType;

        // Reset ou ajuste das propriedades dependendo do tipo da luz
        if (type == LightType::Directional)
        {
            // Para luzes direcionais, a posição não é necessária, então você pode ignorá-la ou resetá-la.
            position = glm::vec3(0.0f);                  // Não necessário para luz direcional
            innerCutoff = glm::cos(glm::radians(12.5f)); // Reset para valores padrões
            outerCutoff = glm::cos(glm::radians(17.5f)); // Reset para valores padrões
            constant = 1.0f;                             // Atenuação padrão
            linear = 0.09f;                              // Atenuação padrão
            quadratic = 0.032f;                          // Atenuação padrão
        }
        else if (type == LightType::Point)
        {
            // Para luzes do tipo Point, é necessário definir a posição.
            // As configurações de Spot, como Cutoffs, podem ser ignoradas ou ajustadas.
            innerCutoff = glm::cos(glm::radians(12.5f)); // Reset para valores padrões
            outerCutoff = glm::cos(glm::radians(17.5f)); // Reset para valores padrões
            constant = 1.0f;                             // Atenuação padrão
            linear = 0.09f;                              // Atenuação padrão
            quadratic = 0.032f;                          // Atenuação padrão
        }
        else if (type == LightType::Spot)
        {
            // Para luzes Spot, além da posição, é necessário definir os ângulos de corte.
            innerCutoff = glm::cos(glm::radians(12.5f)); // Padrão para spot
            outerCutoff = glm::cos(glm::radians(17.5f)); // Padrão para spot
            constant = 1.0f;                             // Atenuação padrão
            linear = 0.09f;                              // Atenuação padrão
            quadratic = 0.032f;                          // Atenuação padrão
        }
    }

    // Use the correct light properties based on the type
    void renderComponent() override
    {
        if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
            const char *lightTypeNames[] = {"Directional", "Point", "Spot"};
            int currentIndex = static_cast<int>(type);

            if (ImGui::Combo("Light Type", &currentIndex, lightTypeNames, IM_ARRAYSIZE(lightTypeNames)))
            {
                // Atualiza a variável com o valor selecionado
                type = static_cast<LightType>(currentIndex);
            }

            ImGui::Separator();

            // Controle de cor
            ImGui::ColorEdit3("Color", &color[0]);

            // Controle de intensidade
            ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f);

            ImGui::Separator();

            // Baseado no tipo de luz, exibe controles adicionais
            switch (type)
            {
            case LightType::Directional:
            {
                // Luz Direcional: apenas a direção
                ImGui::Text("Direction");
                ImGui::DragFloat3("Direction", &direction[0], 0.1f, -360.0, 360.0f);

                break;
            }
            case LightType::Point:
            {
                // Luz Ponto: posição e alcance
                ImGui::Text("Position");
                ImGui::DragFloat3("Position", &position[0], 0.1f);

                ImGui::SliderFloat("Range", &range, 0.0f, 100.0f);

                // Atenuação
                ImGui::SliderFloat("Constant Attenuation", &constant, 0.0f, 10.0f);
                ImGui::SliderFloat("Linear Attenuation", &linear, 0.0f, 10.0f);
                ImGui::SliderFloat("Quadratic Attenuation", &quadratic, 0.0f, 10.0f);

                break;
            }
            case LightType::Spot:
            {
                // Luz Spot: posição, direção, ângulos de corte e alcance
                ImGui::Text("Position");
                ImGui::DragFloat3("Position", &position[0], 0.1f);

                ImGui::Text("Direction");
                ImGui::DragFloat3("Direction", &direction[0], 0.1f, -360.0, 360.0f);

                // Controle para os ângulos de corte do Spot
                ImGui::SliderFloat("Inner Cutoff", &innerCutoff, 0.0f, 1.0f);
                ImGui::SliderFloat("Outer Cutoff", &outerCutoff, 0.0f, 1.0f);

                ImGui::SliderFloat("Range", &range, 0.0f, 100.0f);

                // Atenuação
                ImGui::SliderFloat("Constant Attenuation", &constant, 0.0f, 10.0f);
                ImGui::SliderFloat("Linear Attenuation", &linear, 0.0f, 10.0f);
                ImGui::SliderFloat("Quadratic Attenuation", &quadratic, 0.0f, 10.0f);

                break;
            }
            }
        }
    }
};
