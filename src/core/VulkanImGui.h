#ifndef VULKANIMGUI_H
#define VULKANIMGUI_H

#pragma once
#include "VulkanCore.h"

class VulkanImGui {
public:
    VulkanImGui(VulkanCore& core);
    ~VulkanImGui();
    
    void init(VkRenderPass renderPass);
    void cleanup();
    void render(VkCommandBuffer commandBuffer);
    void newFrame();
    
private:
    void setupDescriptorPool();
    
    VulkanCore& core;
    VkDescriptorPool imguiPool;
};

#endif