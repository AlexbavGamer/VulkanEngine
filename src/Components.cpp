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

void Entity::render(VkCommandBuffer commandBuffer, const CameraComponent &camera)
{
      // Validation check
    if (!material.pipeline || !material.pipelineLayout || !material.descriptorSet || 
        !mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0) {
        return;
    }

    // Set initial transform if not set
    if (transform.scale == glm::vec3(0.0f)) {
        transform.scale = glm::vec3(1.0f);
    }

    // Update matrices
    glm::mat4 modelMatrix = transform.getMatrix();
    
    // Create and update UBO
    UBO ubo{};
    ubo.model = modelMatrix;
    ubo.view = camera.view;
    ubo.projection = camera.projection;
    
    // Update GPU data
    vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(commandBuffer, material.uniformBuffer, ubo);

    // Bind pipeline and descriptors
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout, 0, 1, &material.descriptorSet, 0, nullptr);

    // Bind vertex and index buffers
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // Draw
    vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
}