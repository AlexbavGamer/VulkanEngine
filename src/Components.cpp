#include "Components.h"
#include "VulkanRenderer.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanSwapChain.h"
#include "core/VulkanPipeline.h"

VulkanRenderer& vulkanRender = VulkanRenderer::getInstance();

void camera::updateCamera(CameraComponent &camera, const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
{
    camera.view = glm::lookAt(position, target, up);
    camera.projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    camera.projection[1][1] *= -1;  // Ajuste para Vulkan
}