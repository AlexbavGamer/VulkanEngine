#pragma once
#include <memory>
#include <imgui.h>
#include <vulkan/vulkan.h>

class VulkanCore;
class Entity;

class UIDrawer {
public:
    UIDrawer(VulkanCore* core);
    
    void drawMainMenuBar();
    void drawSceneWindow(VkDescriptorSet sceneDescriptorSet);
    void drawInspectorWindow(std::shared_ptr<Entity>& selectedEntity);
    void drawHierarchyWindow(std::shared_ptr<Entity>& selectedEntity);
    void drawContentBrowser();
    void drawStatisticsWindow(bool& showStatistics);
    void drawDebugWindow(bool& showDebugWindow);
    void drawProjectCreationModal(bool& showCreateProject);
    
private:
    VulkanCore* core;
    void showFileContextMenu(const std::string& filePath, const std::string& id, bool isDirectory);
    void handleFileSelection(const std::string& filename);
};
