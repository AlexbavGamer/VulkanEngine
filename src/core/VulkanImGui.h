#ifndef VULKANIMGUI_H
#define VULKANIMGUI_H

#pragma once
#include "VulkanCore.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "../ecs/Entity.h"

class VulkanImGui {
public:
    VulkanImGui(VulkanCore& core);
    ~VulkanImGui();
    
    void init(VkRenderPass renderPass);
    void cleanup();
    void render(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
    
private:
    void setupDescriptorPool();
    
    VulkanCore& core;
    VkDescriptorPool imguiPool;
    std::shared_ptr<Entity> selectedEntity;
};

#endif