#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "Components.h"


class Scene {
public:
    ~Scene();
    Scene(VulkanRenderer &renderer);

    std::vector<Entity> entities;  // Lista de entidades
    CameraComponent camera;        // Componente de câmera
    std::vector<LightComponent> lights;  // Lista de luzes

    // Função para adicionar uma entidade à cena
    void addEntity(const Entity& entity);

    // Função para renderizar a cena
    void render(VkCommandBuffer commandBuffer);

    void updatePipelineReferences(VkPipeline newPipeline, VkPipelineLayout newLayout);
    
    void updateCamera();

private:
    VulkanRenderer* renderer;
public:
    float cameraRadius = 5.0f;
    float cameraTheta = 0.0f;
    float cameraPhi = 0.0f;
    
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouse = true;
    float mouseSensitivity = 0.005f;
};
