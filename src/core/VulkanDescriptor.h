#pragma once
#include "VulkanCore.h"
#include "VulkanTypes.h"
#include "../Components.h"
#include <array>

class VulkanDescriptor {
public:
    VulkanDescriptor(VulkanCore& core);
    ~VulkanDescriptor();
    
    void create();
    void cleanup();
    void createDescriptorPool();
    void createDescriptorSetLayout();
    void updateDescriptorSets();
    void createDescriptorSets();
    void createUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void updateUniformBuffer(VkCommandBuffer commandBuffer, VkBuffer uniformBuffer, const UBO &ubo);

    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    VkDescriptorSet getDescriptorSet(size_t index) const { return descriptorSets[index]; }
    
private:
    VulkanCore& core;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    
public:
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    void* uniformBufferMapped;
};