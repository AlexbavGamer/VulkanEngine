#include "RenderSystem.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"
#include "../Scene.h"
#include "../VulkanRenderer.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanDescriptor.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

void RenderSystem::render(Registry &registry, VkCommandBuffer commandBuffer)
{
    VulkanRenderer &vulkanRender = VulkanRenderer::getInstance();

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vulkanRender.getCore()->getSwapChain()->getExtent().width;
    viewport.height = (float)vulkanRender.getCore()->getSwapChain()->getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vulkanRender.getCore()->getSwapChain()->getExtent();

    registry.view<RenderComponent, TransformComponent>(
        [&](std::shared_ptr<Entity> entity, RenderComponent &render, TransformComponent &transform)
        {
            const auto &mesh = render.mesh;
            const auto &material = render.material;

            if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0 ||
                !material.descriptorSet || !material.uniformBuffer || !material.pipeline || !material.pipelineLayout)
            {
                return;
            }

            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);


            // Preencher UBO
            UBO ubo = prepareUBO(transform, vulkanRender);
            
            // Atualizar Uniform Buffer
            vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(material.uniformBufferMemory, ubo);

            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
            vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout, 0, 1, &material.descriptorSet, 0, nullptr);
            vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
        });
}

UBO RenderSystem::prepareUBO(const TransformComponent &transform, VulkanRenderer &vulkanRender)
{
    UBO ubo{};
    ubo.model = transform.getMatrix();
    ubo.view = vulkanRender.getCore()->getScene()->camera.view;
    ubo.proj = vulkanRender.getCore()->getScene()->camera.projection;
    ubo.material.color = glm::vec4(1.0f); // Exemplos de valores
    ubo.material.metallic = 0.5f;
    ubo.material.roughness = 0.8f;
    ubo.material.ambientOcclusion = 1.0f;

    return ubo;
}
