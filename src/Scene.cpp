#include "Scene.h"
#include "Components.h"
#include "core/VulkanCore.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanPipeline.h"
#include "core/VulkanSwapChain.h"
#include <iostream>

Scene::Scene(VulkanCore* core) : core(core) {
    registry = std::make_unique<Registry>();
    renderSystem = std::make_unique<RenderSystem>();
}

Scene::~Scene() {}

std::shared_ptr<Entity> Scene::createEntity() {
    return registry->createEntity();
}

void Scene::updateCamera() {
    float x = cameraRadius * sin(cameraTheta) * cos(cameraPhi);
    float y = cameraRadius * sin(cameraPhi);
    float z = cameraRadius * cos(cameraTheta) * cos(cameraPhi);

    glm::vec3 cameraPos(x, y, z);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    camera::updateCamera(camera, cameraPos, target, up);
}

void Scene::handleKeyboardInput(GLFWwindow* window) {
    static bool escPressed = false;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!escPressed) {
            cursorEnabled = !cursorEnabled;
            glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            if (cursorEnabled) {
                firstMouse = true;
            }
        }
        escPressed = true;
    } else {
        escPressed = false;
    }

    if (!cursorEnabled) {
        // Calculate new direction vectors based on yaw and pitch
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        glm::vec3 forward = glm::normalize(direction);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        
        // Extract current position from view matrix
        glm::mat4 view = camera.view;
        glm::vec4 pos = glm::inverse(view)[3];
        glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);

        // Movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += forward * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= forward * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= right * moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * moveSpeed;

        glm::vec3 target = position + forward;
        camera::updateCamera(camera, position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void Scene::handleMouseInput(GLFWwindow* window, double xpos, double ypos)
{
    std::cout << "Mouse position: " << xpos << ", " << ypos << std::endl;
    if(cursorEnabled) return;

    if(firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;
    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch = glm::clamp(pitch + yoffset, -89.0f, 89.0f);
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

void Scene::updateCameraAspect(float aspectRatio) {
    camera.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    camera.projection[1][1] *= -1;
}

void Scene::render(VkCommandBuffer commandBuffer) {
    renderSystem->render(
        *registry,
        commandBuffer,
        core->getPipeline()->getLayout(),
        core->getDescriptor()->getSet(core->getCurrentFrame())
    );
}

void Scene::updatePipelineReferences(VkPipeline newPipeline, VkPipelineLayout newLayout) {
    // Update pipeline references if needed
}
