#pragma once
#include <vulkan/vulkan.h>
#include <vector>

struct DescriptorSetLayout {
    VkDescriptorSetLayout layout;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    bool operator==(const DescriptorSetLayout& other) const {
        return layout == other.layout;
    }
};

namespace std {
    template<>
    struct hash<DescriptorSetLayout> {
        size_t operator()(const DescriptorSetLayout& layout) const {
            return std::hash<VkDescriptorSetLayout>{}(layout.layout);
        }
    };
}
