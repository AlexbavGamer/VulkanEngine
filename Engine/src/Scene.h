#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include "Components.h"
#include "ecs/Registry.h"
#include "ecs/RenderSystem.h"
#ifdef _WIN32
#include <glfw/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
#include "ecs/components/LightComponent.h"


class VulkanCore;
class Scene {
public:
    ~Scene();
    Scene(VulkanCore* core);

    std::unique_ptr<Registry> registry;
    std::unique_ptr<RenderSystem> renderSystem;
    std::shared_ptr<Entity> cameraEntity;

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
    
    CameraComponent getActiveCamera() const
    {
        return cameraEntity->getComponent<CameraComponent>();
    }
    
    void updateCamera();
    void handleMouseInput(GLFWwindow* window, double xpos, double ypos);
    void handleKeyboardInput(GLFWwindow *window);
private:
    VulkanCore* core;
};