#include "UIDrawer.h"
#include "../ecs/Entity.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanPipeline.h"
#include "../project/projectManagment.h"
#include <managers/FileManager.h>

UIDrawer::UIDrawer(VulkanCore &core) : core(core) {}

void UIDrawer::drawMainMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                glfwSetWindowShouldClose(core.getWindow(), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Project"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void UIDrawer::drawSceneWindow(VkDescriptorSet sceneDescriptorSet)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)sceneDescriptorSet, viewportSize, ImVec2(0, 0), ImVec2(1, 1));

    ImGui::End();
    ImGui::PopStyleVar();
}

void UIDrawer::drawInspectorWindow(std::shared_ptr<Entity> &selectedEntity)
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, -1.0f), ImVec2(350.0f, -1.0f));
    ImGui::Begin("Inspector");

    if (selectedEntity)
    {
        for (const auto &[type_index, component] : selectedEntity->getComponents())
        {
            if (component)
            {
                component->renderComponent();
            }
        }
    }

    ImGui::End();
}

void UIDrawer::drawHierarchyWindow(std::shared_ptr<Entity> &selectedEntity)
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, -1.0f), ImVec2(350.0f, -1.0f));
    ImGui::Begin("Hierarchy");
    ImGui::End();
}

void UIDrawer::drawContentBrowser()
{
    ImGui::Begin("Content Browser");
    ImGui::End();
}

void UIDrawer::drawStatisticsWindow(bool &showStatistics)
{
    if (!showStatistics)
        return;

    ImGui::Begin("Statistics", &showStatistics);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();
}

void UIDrawer::drawDebugWindow(bool &showDebugWindow)
{
    if (!showDebugWindow)
        return;

    ImGui::Begin("Debug", &showDebugWindow);
    ImGui::Text("Debug Information");

    static bool wireframeMode = false;
    if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
    {
        core.getPipeline()->setWireframeMode(wireframeMode);
    }

    ImGui::End();
}

void UIDrawer::drawProjectCreationModal(bool &showCreateProject)
{
    if (!showCreateProject)
        return;

    static char projectName[256] = "";
    static char projectPath[1024] = "";

    if (ImGui::BeginPopupModal("Create Project", &showCreateProject, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));
        ImGui::InputText("Project Path", projectPath, IM_ARRAYSIZE(projectPath));

        if (ImGui::Button("Browse"))
        {
            // File dialog implementation
        }

        ImGui::Separator();

        if (ImGui::Button("Create"))
        {
            if (strlen(projectName) > 0 && strlen(projectPath) > 0)
            {
                if (core.getProjectManager()->createProject(projectName, projectPath))
                {
                    showCreateProject = false;
                    memset(projectName, 0, sizeof(projectName));
                    memset(projectPath, 0, sizeof(projectPath));
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            showCreateProject = false;
            memset(projectName, 0, sizeof(projectName));
            memset(projectPath, 0, sizeof(projectPath));
        }

        ImGui::EndPopup();
    }
}

void UIDrawer::showFileContextMenu(const std::string &filePath, const std::string &id, bool isDirectory)
{
    if (ImGui::BeginPopup(id.c_str()))
    {
        if (ImGui::MenuItem("Delete"))
        {
        }
        if (ImGui::MenuItem("Rename"))
        {
        }
        ImGui::EndPopup();
    }
}

void UIDrawer::handleFileSelection(const std::string &filename)
{
    std::string extension = FileManager::getInstance().getFileExtension(filename);

    if (extension == ".txt")
    {
        std::string command = "notepad \"" + filename + "\"";
        system(command.c_str());
    }
}