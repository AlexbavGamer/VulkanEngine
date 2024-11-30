#include "RenderSystem.h"
#include "VulkanRenderer.h"
#include <glm/glm.hpp>
#include "../Scene.h"
#include "../VulkanRenderer.h"

void RenderSystem::render(Registry& registry, VkCommandBuffer commandBuffer) {
    VulkanRenderer& vulkanRender = VulkanRenderer::getInstance();

    registry.view<MeshComponent, MaterialComponent, TransformComponent>([&](std::shared_ptr<Entity> entity, const MeshComponent& mesh, const MaterialComponent& material, TransformComponent& transform) {
        if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0) {
            return;
        }

        // Update UBO
        UBO ubo{};
        ubo.model = transform.getMatrix();
        ubo.view = vulkanRender.getCore()->getScene()->camera.view;
        ubo.proj = vulkanRender.getCore()->getScene()->camera.projection;
        ubo.material.color = material.color;
        ubo.material.metallic = material.metallic;
        ubo.material.roughness = material.roughness;
        ubo.material.ambientOcclusion = material.ambientOcclusion;

        vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(commandBuffer, material.uniformBuffer, ubo);

        // Bind pipeline and descriptor set
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout, 0, 1, &material.descriptorSet, 0, nullptr);

        // Draw
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
        vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
    });
}

