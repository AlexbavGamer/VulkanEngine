#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include "Components.h"
#include "ecs/Registry.h"
#include "ecs/RenderSystem.h"

class VulkanCore;
class Scene {
public:
    ~Scene();
    Scene(VulkanCore* core);

    std::unique_ptr<Registry> registry;
    std::unique_ptr<RenderSystem> renderSystem;
    CameraComponent camera;
    std::vector<LightComponent> lights;

    std::shared_ptr<Entity> createEntity();
    void render(VkCommandBuffer commandBuffer);
    void updatePipelineReferences(VkPipeline newPipeline, VkPipelineLayout newLayout);
    void updateCamera();
    void updateCameraAspect(float aspectRatio);
    
private:
    VulkanCore* core;
public:
    float cameraRadius = 5.0f;
    float cameraTheta = 0.0f;
    float cameraPhi = 0.0f;
    
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouse = true;
    float mouseSensitivity = 0.005f;
};
