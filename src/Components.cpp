#include "Components.h"
#include "VulkanRenderer.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanSwapChain.h"
#include "core/VulkanPipeline.h"

VulkanRenderer& vulkanRender = VulkanRenderer::getInstance();

void camera::updateCamera(CameraComponent &camera, const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
{
    camera.projection = glm::perspective(glm::radians(camera.fov), static_cast<float>(camera.width)/static_cast<float>(camera.height), 0.1f, 100.0f);
    camera.projection[1][1] *= -1;  // Ajuste para Vulkan

    camera.view = glm::lookAt(
        position,
        target,
        up
    );    
}