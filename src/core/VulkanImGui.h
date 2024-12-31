#ifndef VULKANIMGUI_H
#define VULKANIMGUI_H

#pragma once
#include "VulkanCore.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "../ecs/Entity.h"

#define ICON_FA_FOLDER "\uf07b"
#define ICON_FA_ARROW_LEFT "\uf060"
#define ICON_FA_FILE "\uf15b"
#define ICON_FA_IMAGE "\uf03e"
#define ICON_FA_CODE "\uf121"
#define ICON_FA_CUBE "\uf1b2"
#define ICON_FA_PAINT_BRUSH "\uf1fc"

#define ICON_MIN_FA 0xf000
#define ICON_MAX_FA 0xf999

class VulkanRenderer;

class VulkanImGui {
public:
    VulkanImGui(VulkanCore* core);
    ~VulkanImGui();
    
    void init(VkRenderPass renderPass);
    void setupStyles();
    void cleanup();
    void render(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
    
private:
    void setupDescriptorPool();
    void handleFileSelection(const std::string& filename);
    void showFileContextMenu(const std::string &filePath, const std::string id, bool isDirectory);
    VulkanCore* core;
    VulkanRenderer& vulkanRenderer;
    VkDescriptorPool imguiPool;
    std::shared_ptr<Entity> selectedEntity;

private:
    ImFont* iconFont;
};

#endif
