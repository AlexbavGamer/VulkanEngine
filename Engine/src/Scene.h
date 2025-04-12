#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include "Components.h"
#include "ecs/Registry.h"
#include "ecs/RenderSystem.h"
#include <glfw/glfw3.h>
#include "ecs/components/LightComponent.h"


class VulkanCore;
class Scene {
public:
    ~Scene();
    Scene(VulkanCore* core);

    std::unique_ptr<Registry> registry;
    std::unique_ptr<RenderSystem> renderSystem;
    CameraComponent camera;

    void addEntity(std::shared_ptr<Entity> entity);

    template <typename... TArgs>
    std::shared_ptr<Entity> createEntity(TArgs&&... args)
    {
        return registry->createEntity(std::forward<TArgs>(args)...);
    }
    void removeEntity(std::shared_ptr<Entity> entity) 
    {
        registry->removeEntity(entity);
    }
    std::shared_ptr<Entity> createLightEntity(LightComponent::LightType lightType = LightComponent::LightType::Point);
    
    void updatePipelineReferences(VkPipeline newPipeline, VkPipelineLayout newLayout);
    void updateCamera();
    void handleMouseInput(GLFWwindow* window, double xpos, double ypos);
    void handleKeyboardInput(GLFWwindow *window);
    void updateCameraAspect(float aspectRatio);
    void updateMousePosition(double mouseX, double mouseY);
    float getCameraFov() { return cameraFov; }
private:
    VulkanCore* core;
public:
    float cameraRadius = 5.0f;
    float cameraTheta = 0.0f;
    float cameraPhi = 0.0f;
    float cameraFov = 90.0f;

    float yaw = -90.0f;
    float pitch = 0.0f;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    double currentMouseX = 0.0;
    double currentMouseY = 0.0;    
    bool firstMouse = true;
    bool cursorEnabled = true;
    float mouseSensitivity = 0.05f;
    float moveSpeed = 0.1f;
};