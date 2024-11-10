#include "Scene.h"
#include "Components.h"
#include "core/VulkanCore.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanPipeline.h"
#include "core/VulkanSwapChain.h"

Scene::Scene(VulkanCore* core) : core(core) {
    registry = std::make_unique<Registry>();
    renderSystem = std::make_unique<RenderSystem>();
}

Scene::~Scene() {}

Entity Scene::createEntity() {
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
