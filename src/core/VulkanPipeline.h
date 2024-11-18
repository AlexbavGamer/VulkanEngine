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
    void recreatePipelineIfNeeded();
    void createGraphicsPipeline(VkRenderPass renderPass, VkExtent2D extent);

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }
    bool wireframeModeChanged = false;
    bool wireframeMode;
private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    
    VulkanCore& core;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
};

#endif