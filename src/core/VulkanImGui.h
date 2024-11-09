#pragma once
#include "VulkanCore.h"
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

class VulkanImGui {
public:
    VulkanImGui(VulkanCore* core);
    ~VulkanImGui();
    
    void init(VkRenderPass renderPass);
    void cleanup();
    void render(VkCommandBuffer commandBuffer);
    void newFrame();
    
private:
    void setupDescriptorPool();
    
    VulkanCore* core;
    VkDescriptorPool imguiPool;
};