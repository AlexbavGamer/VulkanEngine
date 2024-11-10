#include "RenderSystem.h"
#include "VulkanRenderer.h"
#include <glm/glm.hpp>
#include "../Scene.h"
#include "../VulkanRenderer.h"

void RenderSystem::render(Registry& registry, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet)
{
    VulkanRenderer& vulkanRender = VulkanRenderer::getInstance();

    registry.view<MeshComponent, MaterialComponent, TransformComponent>([&](const Entity& entity, const MeshComponent& mesh, const MaterialComponent& material, TransformComponent& transform) {
        if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0) {
            return;
        }

        if (transform.scale == glm::vec3(0.0f)) {
            transform.scale = glm::vec3(1.0f);           
        }

        glm::mat4 modelMatrix = transform.getMatrix();
        UBO ubo{};
        ubo.model = modelMatrix;
        ubo.view = vulkanRender.getCore()->getScene()->camera.view;
        ubo.projection = vulkanRender.getCore()->getScene()->camera.projection;
            
        vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(commandBuffer, material.uniformBuffer, ubo);

        VkPipeline pipeline = vulkanRender.getCore()->getPipeline()->getPipeline();
        VkPipelineLayout layout = vulkanRender.getCore()->getPipeline()->getLayout();
            
        if (!pipeline || !layout || pipeline == VK_NULL_HANDLE || layout == VK_NULL_HANDLE) {
            return;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &material.descriptorSet, 0, nullptr);

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
        vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
    });
}