#include "Scene.h"
#include "Components.h"
#include "core/VulkanCore.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanPipeline.h"
#include "core/VulkanSwapChain.h"
#include <format>
#include <iostream>
#include <magic_enum.hpp>

Scene::Scene(VulkanCore *core) : core(core)
{
    registry = std::make_unique<Registry>();
    renderSystem = std::make_unique<RenderSystem>();
}

Scene::~Scene() {}

void Scene::addEntity(std::shared_ptr<Entity> entity)
{
    registry->addEntity(entity);
}

std::shared_ptr<Entity> Scene::createLightEntity(LightComponent::LightType lightType)
{
    std::shared_ptr<Entity> lightEntity = createEntity();
    LightComponent &lightComponent = lightEntity->addComponent<LightComponent>();
    lightComponent.setType(lightType);
    lightEntity->setName(std::string(magic_enum::enum_name(lightType).data()).append(" Light"));
    return lightEntity;
}

void Scene::updateCamera()
{
    float x = cameraRadius * sin(cameraTheta) * cos(cameraPhi);
    float y = cameraRadius * sin(cameraPhi);
    float z = cameraRadius * cos(cameraTheta) * cos(cameraPhi);

    glm::vec3 cameraPos(x, y, z);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    camera::updateCamera(cameraEntity->getComponent<CameraComponent>(), cameraPos, target, up);
}

void Scene::handleKeyboardInput(GLFWwindow *window)
{
    static bool escPressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        if (!escPressed)
        {
            cursorEnabled = !cursorEnabled;
            glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            if (cursorEnabled)
            {
                firstMouse = true;
            }
        }
        escPressed = true;
    }
    else
    {
        escPressed = false;
    }

    if (!cursorEnabled)
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        glm::vec3 cameraFront = glm::normalize(direction);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

        // Get current camera position
        glm::mat4 view = cameraEntity->getComponent<CameraComponent>().view;
        glm::vec4 pos = glm::inverse(view)[3];
        glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);

        // Get current speed based on shift key
        float currentSpeed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? runSpeed : moveSpeed;

        // Handle movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += cameraFront * currentSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= cameraFront * currentSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= cameraRight * currentSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += cameraRight * currentSpeed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            position += cameraUp * currentSpeed; // Move up
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            position -= cameraUp * currentSpeed; // Move down

        // Update camera view matrix
        cameraEntity->getComponent<CameraComponent>().position = position;
        cameraEntity->getComponent<CameraComponent>().view = glm::lookAt(position, position + cameraFront, cameraUp);
    }}

void Scene::handleMouseInput(GLFWwindow *window, double xpos, double ypos)
{
    if (cursorEnabled)
        return;

    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;
    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    glm::vec3 cameraFront = glm::normalize(direction);
    glm::mat4 view = cameraEntity->getComponent<CameraComponent>().view;
    glm::vec4 pos = glm::inverse(view)[3];
    glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
    glm::vec3 target = position + cameraFront;

    cameraEntity->getComponent<CameraComponent>().view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Scene::updateMousePosition(double mouseX, double mouseY)
{
    lastMouseX = currentMouseX;
    lastMouseY = currentMouseY;

    // Update current mouse position
    currentMouseX = mouseX;
    currentMouseY = mouseY;

    // Calculate mouse movement delta
    double deltaX = currentMouseX - lastMouseX;
    double deltaY = currentMouseY - lastMouseY;

    // Update camera rotation based on mouse movement
    cameraEntity->getComponent<CameraComponent>().handleMouseMovement(deltaX, deltaY);
}

void Scene::updateCameraAspect(float aspectRatio)
{
    cameraEntity->getComponent<CameraComponent>().projection = glm::perspective(glm::radians(cameraEntity->getComponent<CameraComponent>().fov), aspectRatio, 0.1f, 1000.0f);
    cameraEntity->getComponent<CameraComponent>().projection[1][1] *= -1; // Correção para Vulkan
    
    cameraEntity->getComponent<CameraComponent>().view = glm::lookAt(
        cameraEntity->getComponent<CameraComponent>().position,
        cameraEntity->getComponent<CameraComponent>().position + cameraEntity->getComponent<CameraComponent>().front,
        cameraEntity->getComponent<CameraComponent>().up
    );
}

void Scene::updatePipelineReferences(VkPipeline newPipeline, VkPipelineLayout newLayout)
{
    // Update pipeline references if needed
}
