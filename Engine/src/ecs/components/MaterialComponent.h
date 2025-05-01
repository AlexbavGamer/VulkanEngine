#pragma once
#include "../Component.h"
#include "../../rendering/Texture.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <glm/glm.hpp>

struct MaterialComponent : public Component
{
    MaterialComponent(std::shared_ptr<Entity> owner)
        : Component(owner) {}

    void cleanup(VkDevice device)
    {
        if (pipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            pipelineLayout = VK_NULL_HANDLE;
        }
        
        if (pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
        }
        
        if (descriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
            descriptorSetLayout = VK_NULL_HANDLE;
        }
        
        if (uniformBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, uniformBuffer, nullptr);
            uniformBuffer = VK_NULL_HANDLE;
        }
        
        if (uniformBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, uniformBufferMemory, nullptr);
            uniformBufferMemory = VK_NULL_HANDLE;
        }
        
        if (lightBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, lightBuffer, nullptr);
            lightBuffer = VK_NULL_HANDLE;
        }
        
        if (lightBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, lightBufferMemory, nullptr);
            lightBufferMemory = VK_NULL_HANDLE;
        }
    }

    std::shared_ptr<Texture> albedoMap;
    std::shared_ptr<Texture> normalMap;
    std::shared_ptr<Texture> metallicRoughnessMap;
    std::shared_ptr<Texture> aoMap;
    std::shared_ptr<Texture> emissiveMap;

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    // Mesh Buffer
    VkBuffer uniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory uniformBufferMemory = VK_NULL_HANDLE;

    // Light Buffer
    VkBuffer lightBuffer = VK_NULL_HANDLE;
    VkDeviceMemory lightBufferMemory = VK_NULL_HANDLE;

    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    glm::vec3 emissiveFactor = glm::vec3(0.0f);
};