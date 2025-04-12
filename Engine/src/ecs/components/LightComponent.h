#pragma once

#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <magic_enum.hpp>
#include <boost/hana.hpp>
#include "imgui.h"

struct LightComponent : public Component
{
    LightComponent(std::shared_ptr<Entity> owner)
        : Component(owner) {}
    
    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

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

    BOOST_HANA_DEFINE_STRUCT(LightComponent,
        (LightType, type),
        (glm::vec3, color),
        (float, intensity),
        (float, range),
        (float, innerCutoff),
        (float, outerCutoff),
        (glm::vec3, direction),
        (glm::vec3, position),
        (float, constant),
        (float, linear),
        (float, quadratic)
    );
};
