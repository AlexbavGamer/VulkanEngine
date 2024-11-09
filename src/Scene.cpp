#include "Scene.h"
#include "Components.h"  // Necessário para incluir a definição de Entity
#include "VulkanRenderer.h"

Scene::Scene(VulkanRenderer& renderer) : renderer(&renderer) {
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

Scene::~Scene() {}

void Scene::addEntity(const Entity& entity) {
    entities.push_back(entity);
}

void Scene::render(VkCommandBuffer commandBuffer) {
    // Lógica para renderizar as entidades
    for (auto& entity : entities) {
        entity.render(commandBuffer, camera);
    }
}

void Scene::updatePipelineReferences(VkPipeline newPipeline, VkPipelineLayout newLayout)
{
    for (auto& entity : entities) {
        entity.material.pipeline = newPipeline;
        entity.material.pipelineLayout = newLayout;
    }
}