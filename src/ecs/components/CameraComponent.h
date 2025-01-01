#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

struct CameraComponent : public Component
{
    glm::mat4 projection;
    glm::mat4 view;

    float width;
    float height;

    float yaw = -90.0f;       // Initial yaw angle
    float pitch = 0.0f;       // Initial pitch angle
    float sensitivity = 0.1f; // Mouse sensitivity
    float fov = 90.0f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    void handleMouseMovement(double deltaX, double deltaY)
    {
        yaw += deltaX * sensitivity;
        pitch += deltaY * sensitivity;

        // Constrain pitch to avoid camera flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Calculate new front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        // Update view matrix
        view = glm::lookAt(position, position + front, up);
    }

    glm::mat4 getViewProjection() const
    {
        return projection * view;
    }

    void renderComponent() override
    {
    }
};