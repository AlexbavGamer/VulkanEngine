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
    VkDescriptorSetLayout createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);
    void createDescriptorSets();

    void updateDescriptorSets();
    void updateDescriptorSetWithMultipleTextures(VkDescriptorSet descriptorSet, const VkDescriptorBufferInfo &bufferInfo, const std::vector<VkDescriptorImageInfo> &imageInfos);
    void updateDescriptorSet(VkDescriptorSet descriptorSet,
                             const VkDescriptorBufferInfo &bufferInfo,
                             const VkDescriptorImageInfo &imageInfo);
    void cleanup();

    VkDescriptorBufferInfo getBufferInfo(VkBuffer buffer, VkDeviceSize size);
    VkDescriptorSet allocateDescriptorSet();
    VkDescriptorSet createDescriptorSet();
    DescriptorAllocation  createDescriptorSet(
        const std::vector<VkDescriptorSetLayoutBinding> &bindings,
        const VkDescriptorBufferInfo &bufferInfo,
        const std::vector<VkDescriptorImageInfo> &imageInfos);
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
    std::vector<VkDescriptorPool> getDescriptorPools() const { return descriptorPools; }
    void addDescriptorPool(VkDescriptorPool descriptorPool);
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    VkDescriptorSet getDescriptorSet(size_t index) const { return descriptorSets[index]; }

    void createUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void updateUniformBuffer(VkCommandBuffer commandBuffer, VkBuffer uniformBuffer, const UBO &ubo);

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