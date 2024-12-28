#include "RenderSystem.h"
#include "VulkanRenderer.h"
#include <glm/glm.hpp>
#include "../Scene.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanDescriptor.h"
#include "../VulkanRenderer.h"
#include <glm/gtx/string_cast.hpp>

void RenderSystem::render(Registry &registry, VkCommandBuffer commandBuffer)
{
    VulkanRenderer &vulkanRender = VulkanRenderer::getInstance();

    const auto &camera = VulkanRenderer::getInstance().getCore()->getScene()->camera;

    // Add debug print to track entities
    int entityCount = 0;

    registry.view<RenderComponent, TransformComponent>([&](std::shared_ptr<Entity> entity,
                                                           RenderComponent &render, TransformComponent &transform)
                                                       {
        entityCount++;

        const auto& mesh = render.mesh;
        const auto& material = render.material;

        // Validate resources
        if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0 || 
            !material.descriptorSet || !material.pipelineLayout || !material.pipeline) {
            std::cout << "Missing required resources for rendering" << std::endl;
            return;
        }

        // 1. Bind Pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);

        // 2. Update UBO with current transform and material data
        UBO ubo{};
        ubo.model = transform.getMatrix();
        ubo.view = vulkanRender.getCore()->getScene()->camera.view;
        ubo.proj = vulkanRender.getCore()->getScene()->camera.projection;
        ubo.material.color = material.baseColorFactor;
        ubo.material.metallic = material.metallicFactor;
        ubo.material.roughness = material.roughnessFactor;
        ubo.material.ambientOcclusion = 1.0f;

        // 3. Update Uniform Buffer
        vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(
            commandBuffer,
            material.uniformBuffer,
            ubo
        );

        // 4. Bind Descriptor Sets
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

        // 5. Bind vertex and index buffers
        VkBuffer vertexBuffers[] = {mesh.vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        // 6. Draw
        vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0); });
}
