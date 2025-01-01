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

std::shared_ptr<Entity> Scene::createEntity()
{
    return registry->createEntity();
}

std::shared_ptr<Entity> Scene::createLightEntity(LightComponent::LightType lightType)
{
    std::shared_ptr<Entity> lightEntity = createEntity();
    LightComponent &lightComponent = lightEntity->addComponent<LightComponent>();
    lightComponent.setType(lightType);
    RenderComponent &renderComponent = lightEntity->addComponent<RenderComponent>();
    renderComponent.name = std::string(magic_enum::enum_name(lightType).data()).append(" Light");
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

    camera::updateCamera(camera, cameraPos, target, up);
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
        glm::mat4 view = camera.view;
        glm::vec4 pos = glm::inverse(view)[3];
        glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);

        // Handle movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += cameraFront * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= cameraFront * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= cameraRight * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += cameraRight * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            position += cameraUp * moveSpeed; // Move up
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            position -= cameraUp * moveSpeed; // Move down

        // Update camera view matrix
        camera.position = position;
        camera.view = glm::lookAt(position, position + cameraFront, cameraUp);
    }
}

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
    glm::mat4 view = camera.view;
    glm::vec4 pos = glm::inverse(view)[3];
    glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
    glm::vec3 target = position + cameraFront;

    camera.view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
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
    camera.handleMouseMovement(deltaX, deltaY);
}

void Scene::updateCameraAspect(float aspectRatio)
{
    camera.projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    camera.projection[1][1] *= -1;
}

void Scene::updatePipelineReferences(VkPipeline newPipeline, VkPipelineLayout newLayout)
{
    // Update pipeline references if needed
}
