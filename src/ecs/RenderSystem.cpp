#include "RenderSystem.h"
#include "VulkanRenderer.h"
#include <glm/glm.hpp>
#include "../Scene.h"
#include "../VulkanRenderer.h"

void RenderSystem::render(Registry& registry, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet)
{
    VulkanRenderer& vulkanRender = VulkanRenderer::getInstance();

    registry.view<MeshComponent, MaterialComponent, TransformComponent>([&](std::shared_ptr<Entity> entity, const MeshComponent& mesh, const MaterialComponent& material, TransformComponent& transform) 
    {
        if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0) {
            return;
        }

        // World space transform calculation
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, transform.position);

        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, transform.scale);

        UBO ubo{};
        ubo.model = modelMatrix;
        ubo.view = vulkanRender.getCore()->getScene()->camera.view;
        ubo.projection = vulkanRender.getCore()->getScene()->camera.projection;
        
        // Correct normal matrix calculation for lighting
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        ubo.normalMatrix = normalMatrix;
        
        // Sun position calculation
        float time = glfwGetTime();
        float radius = 15.0f;
        glm::vec3 lightPos = glm::vec3(
            cos(time * 0.1f) * radius,
            10.0f + sin(time * 0.1f) * 5.0f,
            sin(time * 0.1f) * radius
        );

        ubo.lightPosition = lightPos;
        ubo.lightColor = vulkanRender.getCore()->getScene()->lights[0].color;
        ubo.viewPos = vulkanRender.getCore()->getScene()->camera.position;
        
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
