#include "Components.h"
#include "VulkanRenderer.h"
#include "core/VulkanCore.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanSwapChain.h"
#include "core/VulkanPipeline.h"

const VulkanRenderer& vulkanRender = VulkanRenderer::getInstance();

void camera::updateCamera(CameraComponent &camera, const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up)
{
    float aspectRatio = static_cast<float>(camera.width) / static_cast<float>(camera.height);
    camera.projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    camera.projection[1][1] *= -1;
    
    camera.position = position;
    camera.front = glm::normalize(target - position);
    camera.up = up;
    
    camera.view = glm::lookAt(position, position + camera.front, up);
}
