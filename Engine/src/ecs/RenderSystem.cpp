#include "RenderSystem.h"
#include "components/TransformComponent.h"
#include "../Scene.h"
#include "../VulkanRenderer.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanDescriptor.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

void RenderSystem::render(Registry& registry, VkCommandBuffer commandBuffer)
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

    // Set viewport and scissor once for all entities
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Recursive function to render an entity and all its children
    std::function<void(std::shared_ptr<Entity>)> renderEntityHierarchy = 
        [&](std::shared_ptr<Entity> entity) {
            // Render the entity if it has the required components
            if (entity->hasComponent<MeshComponent>() && 
                entity->hasComponent<MaterialComponent>() && 
                entity->hasComponent<TransformComponent>()) {
                
                const auto& mesh = entity->getComponent<MeshComponent>();
                const auto& material = entity->getComponent<MaterialComponent>();
                const auto& transform = entity->getComponent<TransformComponent>();
                
                if (!mesh.vertexBuffer || !mesh.indexBuffer || mesh.indexCount == 0 ||
                    !material.descriptorSet || !material.uniformBuffer || !material.pipeline || !material.pipelineLayout) {
                    // Skip entities with invalid components
                }
                else {
                    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);

                    // Prepare UBO
                    UBO ubo = prepareUBO(transform, vulkanRender);

                    // Update Uniform Buffer
                    vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(material.uniformBufferMemory, ubo);

                    LightUBO lightUBO = prepareLightUBO(vulkanRender);
                    vulkanRender.getCore()->getDescriptor()->updateUniformBuffer(material.lightBufferMemory, lightUBO);

                    VkDeviceSize offsets[] = {0};
                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
                    vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout, 0, 1, &material.descriptorSet, 0, nullptr);
                    vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
                }
            }
            
            // Recursively render all children
            for (const auto& child : entity->getChildren()) {
                auto childEntity = std::dynamic_pointer_cast<Entity>(child);
                if (childEntity) {
                    renderEntityHierarchy(childEntity);
                }
            }
        };

    // Get all entities from the registry
    auto entities = registry.getEntities();
    
    // Render only root entities (those without parents)
    for (auto& entity : entities) {
        // Check if this is a root entity (no parent)
        if (!entity->getParent()) {
            renderEntityHierarchy(entity);
        }
    }
}

UBO RenderSystem::prepareUBO(const TransformComponent &transform, VulkanRenderer &vulkanRender)
{
    UBO ubo{};
    ubo.model = transform.getWorldMatrix();
    ubo.view = vulkanRender.getCore()->getScene()->cameraEntity->getComponent<CameraComponent>().view;
    ubo.proj = vulkanRender.getCore()->getScene()->cameraEntity->getComponent<CameraComponent>().projection;
    ubo.material.color = glm::vec4(1.0f); // Exemplos de valores
    ubo.material.metallic = 0.5f;
    ubo.material.roughness = 0.8f;
    ubo.material.ambientOcclusion = 1.0f;

    return ubo;
}

LightUBO RenderSystem::prepareLightUBO(VulkanRenderer &vulkanRender)
{
    LightUBO lightUBO{};

    auto scene = vulkanRender.getCore()->getScene();
    auto lights = scene->registry->viewWithSpecificComponents<LightComponent>();
    lightUBO.numLights = lights.size();

    for (size_t i = 0; i < lightUBO.numLights; ++i)
    {
        const LightComponent &lightComponent = lights[i]->getComponent<LightComponent>();
        GPULight gpuLight{
            .position = lightComponent.position,
            .direction = lightComponent.direction,
            .color = lightComponent.color,
            .intensity = lightComponent.intensity,
            .range = lightComponent.range,
            .innerCutoff = lightComponent.innerCutoff,
            .outerCutoff = lightComponent.outerCutoff,
            .constant = lightComponent.constant,
            .linear = lightComponent.linear,
            .quadratic = lightComponent.quadratic,
            .type = static_cast<int>(lightComponent.type)};
        lightUBO.lights[i] = gpuLight;
    }

    return lightUBO;
}