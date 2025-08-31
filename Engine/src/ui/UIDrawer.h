#pragma once
#include <memory>
#include <imgui.h>
#include <vulkan/vulkan.h>
#include "ImGuizmo.h"
#include <glm/glm.hpp>
#include <string_view>

class VulkanCore;
class Entity;

class UIDrawer
{
public:
    UIDrawer(VulkanCore *core);

    void drawMainMenuBar();
    void drawSceneWindow(VkDescriptorSet sceneDescriptorSet, std::shared_ptr<Entity> &selectedEntity);
    void drawInspectorWindow(std::shared_ptr<Entity> &selectedEntity);
    void drawHierarchyWindow(std::shared_ptr<Entity> &selectedEntity);
    void drawContentBrowser();
    void drawStatisticsWindow();
    void drawDebugWindow();
    void drawProjectCreationModal();
    void drawOpenProjectModal();

private:
    VulkanCore *core;
    void renderGuizmo(std::shared_ptr<Entity> &selectedEntity);
    void showFileContextMenu(const std::string &filePath, const std::string &id, bool isDirectory);
    void handleFileSelection(const std::string &filename);
    void DrawEntityNode(std::shared_ptr<Entity> &selectedEntity, std::shared_ptr<Entity> &entity);
    template <typename T>
    void DrawInspector(T& obj, const std::string_view& title = typeid(T).name());
    void handleGizmoOperations();
    void updateEntityTransform(std::shared_ptr<Entity>& selectedEntity);
    void setupImGuizmoView(const ImVec2& windowPos, const ImVec2& windowSize, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
    bool manipulateTransform(std::shared_ptr<Entity>& selectedEntity, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
private:
    const char *createProjectFileDialogKey = "ChooseProjectDirectoryKey";
    const char *openProjectFileDialogKey = "OpenProjectFileKey";

    bool showStatistics = false;
    bool showDebugWindow = false;
    bool showCreateProject = false;
    bool showOpenProject = false;

    ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
private:
    template<typename T>
    void DrawField(const std::string_view& label, T& value);
};