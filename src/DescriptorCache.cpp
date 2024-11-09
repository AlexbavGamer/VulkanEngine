#include "DescriptorCache.h"

VkDescriptorSet DescriptorCache::getOrCreate(const DescriptorSetLayout& layout) {
    size_t hash = std::hash<DescriptorSetLayout>{}(layout);
    
    auto it = cache.find(hash);
    if (it != cache.end()) {
        return it->second;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout.layout;

    VkDescriptorSet descriptorSet;
    vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
    
    cache[hash] = descriptorSet;
    return descriptorSet;
}
