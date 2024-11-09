#pragma once
#include <vulkan/vulkan.h>

class DynamicUniformBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
    size_t alignedSize;
    void* mapped;
public:
    void update(uint32_t index, const void* data);
};
