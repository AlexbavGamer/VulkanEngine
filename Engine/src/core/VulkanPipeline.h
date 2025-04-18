#ifndef VULKANPIPELINE_H
#define VULKANPIPELINE_H

#pragma once
#include "VulkanCore.h"
#include "VulkanTypes.h"
#include "VulkanSwapChain.h"
#include <memory>

class VulkanPipeline {
public:
    VulkanPipeline(VulkanCore& core);
    ~VulkanPipeline();
    
    void create(VkRenderPass renderPass, VkExtent2D extent);
    void cleanup();
    void recreate(VkRenderPass renderPass, VkExtent2D extent);
    void setWireframeMode(bool enabled);
    void createGraphicsPipeline(VkRenderPass renderPass, VkExtent2D extent);
    void createScenePipeline(VkRenderPass renderPass, VkExtent2D extent);
    
    void bindScenePipeline(VkCommandBuffer commandBuffer);

    VkPipeline createMaterialPipeline(
        VkPipelineLayout& outPipelineLayout,
        VkDescriptorSetLayout descriptorSetLayout,
        const std::string& vertShaderPath,
        const std::string& fragShaderPath
    );

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipeline getScenePipeline() const { return sceneGraphicsPipeline; }
    VkPipelineLayout getScenePipelineLayout() const { return scenePipelineLayout; }
    VkPipelineLayout getUIPipelineLayout() const { return uiPipelineLayout; }
    
    bool wireframeModeChanged = false;
    bool wireframeMode;
private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    
    VulkanCore& core;
    VkPipeline graphicsPipeline;
    VkPipeline sceneGraphicsPipeline;
    VkPipelineLayout scenePipelineLayout;
    VkPipelineLayout uiPipelineLayout;
};

#endif