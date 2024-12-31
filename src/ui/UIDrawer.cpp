#include "UIDrawer.h"
#include "../ecs/Entity.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanPipeline.h"
#include "../project/projectManagment.h"
#include "Scene.h"
#include <managers/FileManager.h>
#include <ecs/components/RenderComponent.h>
#include <iostream>

#include "ImGuiFileDialog.h"

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
                showCreateProject = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open"))
            {
                showOpenProject = true;
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
        ImGui::Text("Project Name: %s", core->getProjectManager()->getConfig().projectName.c_str());
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
    if (showCreateProject)
    {
        ImGui::OpenPopup("Create Project");
        showCreateProject = false; // Reset após abrir
    }

    bool isOpen = true;
    if (ImGui::BeginPopupModal("Create Project", &isOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char projectName[256] = "";
        static char projectPath[1024] = "";

        ImGui::Text("Project Settings");
        ImGui::Separator();

        ImGui::Text("Project Name:");
        ImGui::InputText("##projectname", projectName, IM_ARRAYSIZE(projectName));

        ImGui::Text("Project Location:");
        ImGui::InputText("##projectpath", projectPath, IM_ARRAYSIZE(projectPath));
        ImGui::SameLine();

        if (ImGui::Button("Browse..."))
        {
            ImGuiFileDialog::Instance()->OpenDialog(createProjectFileDialogKey,
                                                    "Choose Directory",
                                                    nullptr,
                                                    {.flags = ImGuiFileDialogFlags_Modal});
        }

        ImGui::Separator();

        if (ImGuiFileDialog::Instance()->Display(createProjectFileDialogKey,
                                                 ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            // Quando o usuário selecionar um diretório
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
                strncpy(projectPath, path.c_str(), sizeof(projectPath) - 1);
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            core->getProjectManager()->createProject(projectName, projectPath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UIDrawer::drawOpenProjectModal()
{
    if (showOpenProject)
    {
        ImGui::OpenPopup("Open Project");
        showOpenProject = false; // Reset após abrir
    }

    bool isOpen = true;
    if (ImGui::BeginPopupModal("Open Project", &isOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char projectPath[1024] = "";

        ImGui::Text("Select Project File");
        ImGui::Separator();

        ImGui::InputText("##projectpath", projectPath, IM_ARRAYSIZE(projectPath));
        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            ImGuiFileDialog::Instance()->OpenDialog(openProjectFileDialogKey,
                                                    "Choose Directory",
                                                    nullptr,
                                                    {.flags = ImGuiFileDialogFlags_Modal});
        }

        if (ImGuiFileDialog::Instance()->Display(openProjectFileDialogKey,
                                                 ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            // Quando o usuário selecionar um diretório
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
                strncpy(projectPath, path.c_str(), sizeof(projectPath) - 1);
            }
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::Separator();

        if (ImGui::Button("Open", ImVec2(120, 0)))
        {
            core->getProjectManager()->openProject(projectPath);
            ImGui::CloseCurrentPopup();

            std::cout << "Project Path: " << projectPath << std::endl;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
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