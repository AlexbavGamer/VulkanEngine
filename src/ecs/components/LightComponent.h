#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

struct LightComponent : public Component
{
    enum class Type
    {
        Directional,
        Point,
        Spot
    };

    Type type = Type::Point;
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float range = 10.0f;

    // For spot lights
    float innerCutoff = glm::cos(glm::radians(12.5f));
    float outerCutoff = glm::cos(glm::radians(17.5f));

    // For directional lights
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

    // Attenuation factors
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    void renderComponent() override
    {
        
    }
};