#pragma once
#include "VulkanCore.h"
#include "VulkanTypes.h"
#include "../Components.h"
#include <array>

struct DescriptorAllocation
{
    VkDescriptorSet set;
    VkDescriptorPool pool;
};

class VulkanDescriptor
{
public:
    VulkanDescriptor(VulkanCore &core);
    ~VulkanDescriptor();

    void create();
    void createDescriptorPool();
    void createDescriptorSetLayout();
    void createDescriptorSets();

    void cleanup();

    VkDescriptorBufferInfo getBufferInfo(VkBuffer buffer, VkDeviceSize size);
    VkDescriptorSet allocateDescriptorSet();
    VkDescriptorSet createDescriptorSet();
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
    std::vector<VkDescriptorPool> getDescriptorPools() const { return descriptorPools; }
    void addDescriptorPool(VkDescriptorPool descriptorPool);
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    VkDescriptorSet getDescriptorSet(size_t index) const { return descriptorSets[index]; }

    std::vector<VkDescriptorSetLayoutBinding> getDescriptorSetLayoutBindings() const;

    void createUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void updateUniformBuffer(VkDeviceMemory uniformBufferMemory, const UBO &ubo);

private:
    VulkanCore &core;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDescriptorPool> descriptorPools;

public:
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    void *uniformBufferMapped;
};