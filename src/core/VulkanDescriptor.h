#ifndef VULKANDESCRIPTOR_H
#define VULKANDESCRIPTOR_H

#pragma once
#include "VulkanCore.h"
#include "VulkanTypes.h"
#include "../Components.h"
#include <array>

class VulkanDescriptor {
public:
    VulkanDescriptor(VulkanCore* core);
    ~VulkanDescriptor();
    
    void create();
    void cleanup();
    void createPool();
    void createSetLayout();
    void createSets();
    void createUniformBuffer();
    void updateUniformBuffer(VkCommandBuffer commandBuffer, VkBuffer uniformBuffer, const UBO &ubo);

    VkDescriptorSetLayout getSetLayout() const { return descriptorSetLayout; }
    VkDescriptorSet getSet(size_t index) const { return descriptorSets[index]; }
    
private:
    VulkanCore* core;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
    void* uniformBufferMapped;
};


#endif