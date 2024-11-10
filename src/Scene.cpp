#include "Scene.h"
#include "Components.h"  // Necessário para incluir a definição de Entity
#include "core/VulkanCore.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanPipeline.h"
#include "core/VulkanSwapChain.h"

Scene::Scene(VulkanCore* core) : core(core)
{

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

void Scene::updateCameraAspect(float aspectRatio)
{
    camera.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    camera.projection[1][1] *= -1;
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