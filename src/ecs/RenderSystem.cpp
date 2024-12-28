#include "RenderSystem.h"
#include "VulkanRenderer.h"
#include <glm/glm.hpp>
#include "../Scene.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanDescriptor.h"
#include "../VulkanRenderer.h"
#include <glm/gtx/string_cast.hpp>

void RenderSystem::render(Registry &registry, VkCommandBuffer commandBuffer) {
    VulkanRenderer &vulkanRender = VulkanRenderer::getInstance();

    registry.view<RenderComponent, TransformComponent>([&](
        std::shared_ptr<Entity> entity,
        RenderComponent &render,
        TransformComponent &transform) {
            const auto &mesh = render.mesh;
            const auto &material = render.material;

            if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0 ||
                !material.descriptorSet || !material.pipelineLayout || !material.pipeline) {
                return;
            }

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);

            UBO ubo{};
            ubo.model = transform.getMatrix();
            ubo.view = vulkanRender.getCore()->getScene()->camera.view;
            ubo.proj = vulkanRender.getCore()->getScene()->camera.projection;
            ubo.material.color = material.baseColorFactor;
            ubo.material.metallic = material.metallicFactor;
            ubo.material.roughness = material.roughnessFactor;
            ubo.material.ambientOcclusion = 1.0f;

            vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(
                commandBuffer,
                vulkanRender.getCore()->getSceneUniformBuffer(),
                ubo
            );

            vkCmdBindDescriptorSets(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                material.pipelineLayout,
                0,
                1,
                &material.descriptorSet,
                0,
                nullptr
            );

            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
            vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
    });
}