#pragma once

#include "../Component.h"
#include <vulkan/vulkan.h>

struct MeshComponent : public Component
{
    MeshComponent(std::shared_ptr<Entity> owner)
        : Component(owner) {}

    void Destroy(VkDevice device)
    {
        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vkFreeMemory(device, vertexBufferMemory, nullptr);
        
        vkDestroyBuffer(device, indexBuffer, nullptr);
        vkFreeMemory(device, indexBufferMemory, nullptr);
        
        indexCount = 0;
    }

    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
};