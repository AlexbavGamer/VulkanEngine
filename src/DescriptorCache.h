#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "DescriptorSetLayout.h"

class DescriptorCache {
    std::unordered_map<size_t, VkDescriptorSet> cache;
    VkDescriptorPool pool;
    VkDevice device;

public:
    DescriptorCache(VkDevice device) : device(device) {}
    VkDescriptorSet getOrCreate(const DescriptorSetLayout& layout);
};
