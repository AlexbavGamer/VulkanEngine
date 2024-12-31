#include "UIDrawer.h"
#include "../ecs/Entity.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanPipeline.h"
#include "../project/projectManagment.h"
#include "Scene.h"
#include <managers/FileManager.h>
#include <ecs/components/RenderComponent.h>
#include <iostream>

UIDrawer::UIDrawer(VulkanCore *core) : core(core) {}

void UIDrawer::drawMainMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                glfwSetWindowShouldClose(core->getWindow(), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Project"))
        {
            if (ImGui::MenuItem("New"))
            {
                if(showOpenProject)
                {
                    showOpenProject = false;
                }
                ImGui::OpenPopup("Create Project");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open"))
            {
                if(showCreateProject)
                {
                    showCreateProject = false;
                }
                showCreateProject = true;
                ImGui::OpenPopup("Open Project");
            }
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

    // Verifica se clicou fora da janela
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
    {
        selectedEntity = nullptr;
    }

    core->getScene()->registry->view<RenderComponent>([&](std::shared_ptr<Entity> entity, RenderComponent &render)
                                                      {
        std::string selectableLabel = render.name + "##" + std::to_string(entity->getId());
        
        if (ImGui::Selectable(selectableLabel.c_str(), selectedEntity && selectedEntity->getId() == entity->getId())) {
            if (selectedEntity && selectedEntity->getId() == entity->getId()) {
                selectedEntity = nullptr;
            } else {
                selectedEntity = entity;
            }
        } });

    ImGui::End();
}

void UIDrawer::drawContentBrowser()
{
    ImGui::Begin("Content Browser");
    auto &FileManager = FileManager::getInstance();
    if (core->getProjectManager()->isProjectOpen())
    {
    }
    ImGui::End();
}

void UIDrawer::drawStatisticsWindow()
{
    if (!showStatistics)
        return;

    ImGui::Begin("Statistics", &showStatistics);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();
}

void UIDrawer::drawDebugWindow()
{
    if (!showDebugWindow)
        return;

    ImGui::Begin("Debug", &showDebugWindow);
    ImGui::Text("Debug Information");

    static bool wireframeMode = false;
    if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
    {
        core->getPipeline()->setWireframeMode(wireframeMode);
    }

    ImGui::End();
}

void UIDrawer::drawProjectCreationModal()
{
    std::cout << "showCreateProject: " << showCreateProject << std::endl;
    if (ImGui::BeginPopupModal("Create Project", &showCreateProject, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Farofa 4321");
        ImGui::Text("Farofa 4321");
        ImGui::Text("Farofa 4321");
        ImGui::Text("Farofa 4321");
        ImGui::EndPopup();
    }
}

void UIDrawer::drawOpenProjectModal()
{
    if (ImGui::BeginPopupModal("Open Project"))
    {
        ImGui::Text("Farofa 1234");
        ImGui::Text("Farofa 1234");
        ImGui::Text("Farofa 1234");
        ImGui::Text("Farofa 1234");

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