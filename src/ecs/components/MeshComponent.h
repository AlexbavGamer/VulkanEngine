#pragma once

#include "../Component.h"
#include <vulkan/vulkan.h>

struct MeshComponent : public Component
{
    MeshComponent(std::shared_ptr<Entity> owner)
        : Component(owner) {}

    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
};