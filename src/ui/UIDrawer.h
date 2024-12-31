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
    void drawStatisticsWindow();
    void drawDebugWindow();
    void drawProjectCreationModal();
    void drawOpenProjectModal();
private:
    VulkanCore* core;
    void showFileContextMenu(const std::string& filePath, const std::string& id, bool isDirectory);
    void handleFileSelection(const std::string& filename);

    bool showStatistics = false;
    bool showDebugWindow = false;
    bool showCreateProject = false;
    bool showOpenProject = true;
};
