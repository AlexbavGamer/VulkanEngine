#include "../Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <boost/hana.hpp>

struct CameraComponent : public Component
{
    CameraComponent(std::shared_ptr<Entity> owner = nullptr) : Component(owner) {
        this->projection = glm::mat4(1.0f);
        this->view = glm::mat4(1.0f);
        this->width = 800.0f;
        this->height = 600.0f;

        this->yaw = -90.0f;
        this->pitch = 0;
        this->sensitivity = 0.1f;
        this->fov = 90.0f;

        this->position = glm::vec3(0.0f, 0.0f, 3.0f);
        this->front = glm::vec3(0.0f, 0.0f, -1.0f);
        this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    }

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

    glm::mat4 getViewMatrix() const {
        return view;
    }

    glm::mat4 getProjectionMatrix() const
    {
        return projection;
    }

    glm::mat4 getViewProjection() const
    {
        return projection * view;
    }

    BOOST_HANA_DEFINE_STRUCT(CameraComponent,
        (glm::mat4, projection),
        (glm::mat4, view),
        (float, width),
        (float, height),
        (float, yaw),
        (float, pitch),
        (float, sensitivity),
        (float, fov),
        (glm::vec3, position),
        (glm::vec3, front),
        (glm::vec3, up)
    );
};