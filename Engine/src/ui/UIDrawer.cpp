#include "UIDrawer.h"
#include "../ecs/Entity.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanPipeline.h"
#include "../project/projectManagment.h"
#include "Scene.h"
#include <managers/FileManager.h>
#include <ecs/components/TransformComponent.h>
#include <ecs/components/LightComponent.h>
#include <iostream>
#include <boost/hana.hpp>
#include "ImGuiFileDialog.h"
#include <core/VulkanImGui.h>
#include <glm/gtc/type_ptr.hpp>

namespace hana = boost::hana;

template <typename T>
constexpr bool is_hana_struct_v = hana::Struct<std::decay_t<T>>::value;


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

void UIDrawer::drawSceneWindow(VkDescriptorSet sceneDescriptorSet, std::shared_ptr<Entity> &selectedEntity)
{
    // Configuração inicial da janela da cena
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Configuração da viewport
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImVec2 viewportPos = ImGui::GetWindowContentRegionMin();
    viewportPos.x += ImGui::GetWindowPos().x;
    viewportPos.y += ImGui::GetWindowPos().y;

    // Renderiza a textura da cena
    ImGui::Image((ImTextureID)sceneDescriptorSet, viewportSize, ImVec2(0, 0), ImVec2(1, 1));

    // Manipulação do Gizmo
    if (selectedEntity && selectedEntity->hasComponent<TransformComponent>())
    {
        handleGizmoOperations();
        setupGizmo(viewportPos, viewportSize);
        updateEntityTransform(selectedEntity);
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void UIDrawer::handleGizmoOperations()
{
    if (ImGui::IsKeyPressed(ImGuiKey_W))
        currentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E))
        currentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R))
        currentGizmoOperation = ImGuizmo::SCALE;
}

void UIDrawer::setupGizmo(const ImVec2& viewportPos, const ImVec2& viewportSize)
{
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
}

void UIDrawer::updateEntityTransform(std::shared_ptr<Entity>& selectedEntity)
{
    auto& transform = selectedEntity->getComponent<TransformComponent>();
    auto& camera = core->getScene()->cameraEntity->getComponent<CameraComponent>();

    glm::mat4 projection = camera.getProjectionMatrix();
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 model = transform.getLocalMatrix();

    // Configura o modo de operação e espaço do Gizmo
    static ImGuizmo::MODE mode = ImGuizmo::WORLD;
    if (ImGui::IsKeyPressed(ImGuiKey_T))
        mode = (mode == ImGuizmo::WORLD) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

    // Aplicar a manipulação do Gizmo com snap opcional
    float snap[3] = { 1.0f, 1.0f, 1.0f }; // Valores de snap para cada operação
    bool useSnap = ImGui::IsKeyPressed(ImGuiKey_LeftCtrl);
    
    bool manipulated = ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(projection),
        currentGizmoOperation,
        mode,
        glm::value_ptr(model),
        nullptr,
        useSnap ? snap : nullptr
    );

    if (manipulated)
    {
        glm::vec3 translation, rotation, scale;
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), 
            glm::value_ptr(translation), 
            glm::value_ptr(rotation), 
            glm::value_ptr(scale));

        // Atualiza a transformação mantendo as coordenadas locais
        transform.setPosition(translation);
        transform.setRotation(glm::radians(rotation));
        transform.setScale(scale);

        // Força a atualização da matriz local
        transform.updateLocalMatrix();
    }
}

void UIDrawer::drawInspectorWindow(std::shared_ptr<Entity> &selectedEntity)
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, -1.0f), ImVec2(350.0f, -1.0f));
    ImGui::Begin("Inspector");

    if (selectedEntity)
    {
        // Display entity name and ID
        ImGui::Text("Entity: %s", selectedEntity->getName().c_str());
        ImGui::Separator();
        
        // Display entity components
        if (selectedEntity->hasComponent<TransformComponent>())
        {
            auto& transform = selectedEntity->getComponent<TransformComponent>();
            DrawInspector(transform, "Transform");
        }

        if (selectedEntity->hasComponent<LightComponent>())
        {
            auto& lightComponent = selectedEntity->getComponent<LightComponent>();
            DrawInspector(lightComponent, "Light Component");
        }

        if(selectedEntity->hasComponent<CameraComponent>())
        {
            auto& cameraComponent = selectedEntity->getComponent<CameraComponent>();
            DrawInspector(cameraComponent, "Camera Component");
        }


        
        // Add other component types here
        // For example:
        // if (selectedEntity->hasComponent<RenderComponent>())
        // {
        //     auto& render = selectedEntity->getComponent<RenderComponent>();
        //     DrawInspector(render, "Render");
        // }
        
        // Add a button to add components
        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }
        
        if (ImGui::BeginPopup("AddComponentPopup"))
        {





            // List available components to add
            if (ImGui::MenuItem("Transform Component") && !selectedEntity->hasComponent<TransformComponent>())
            {
                selectedEntity->addComponent<TransformComponent>();
            }
            // Add other component types here
            
            ImGui::EndPopup();
        }
    }
    else
    {
        ImGui::Text("No entity selected");
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

    core->getScene()->registry->view([&](std::shared_ptr<Entity> entity)
                                     { DrawEntityNode(selectedEntity, entity); });

    ImGui::End();
}

void UIDrawer::drawContentBrowser()
{
    if (ImGui::Begin("Content Browser"))
    {
        if (!core->getProjectManager()->isProjectOpen())
        {
            return ImGui::End();
        }
        ImGui::End();
    }
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
            ImGuiFileDialog::Instance()->OpenDialog(createProjectFileDialogKey, "Choose Directory", nullptr, {.flags = ImGuiFileDialogFlags_Modal});
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
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UIDrawer::renderGuizmo(std::shared_ptr<Entity> &selectedEntity)
{
    // if (selectedEntity)
    // {
    //     glm::mat4 projection = core->getScene()->camera.getViewProjection();
    //     glm::mat4 view = core->getScene()->camera.getViewMatrix();

    //     if (selectedEntity->hasComponent<TransformComponent>())
    //     {
    //         ImGui::SameLine();
    //         ImGui::Text("Opa");
    //         // glm::mat4 model = selectedEntity->getComponent<TransformComponent>().getLocalMatrix();

    //         // ImGuizmo::BeginFrame();

    //         // // Configura o tamanho da área onde o Gizmo será renderizado
    //         // ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    //         // // Renderiza o manipulador de visualização
    //         // ImGuizmo::ViewManipulate(glm::value_ptr(view), 45.0f, ImVec2(10, 10), ImVec2(100, 100), 0x10101010);

    //         // // Manipula a transformação do objeto
    //         // bool manip = ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
    //         //                                   ImGuizmo::TRANSLATE, ImGuizmo::LOCAL,
    //         //                                   glm::value_ptr(model), nullptr, nullptr);

    //         // selectedEntity->getComponent<TransformComponent>().setWorldMatrix(model);
    //     }
    // }
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

void UIDrawer::DrawEntityNode(std::shared_ptr<Entity> &selectedEntity, std::shared_ptr<Entity> &entity)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
    if (entity->getChildren().empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    if (selectedEntity == entity)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool open = ImGui::TreeNodeEx((void *)entity.get(), flags, "%s", entity->getName().c_str());

    if (ImGui::IsItemClicked())
    {
        selectedEntity = entity;
    }

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete"))
        {
            // core->getScene()->removeEntity(entity);
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginDragDropSource())
    {
        auto ptr = entity.get();
        ImGui::SetDragDropPayload("ENTITY", &ptr, sizeof(Entity *));
        ImGui::Text("Movendo %s", entity->getName().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            Entity* droppedEntityPtr = *(Entity**)payload->Data;
            std::shared_ptr<Entity> droppedEntity = std::dynamic_pointer_cast<Entity>(droppedEntityPtr->shared_from_this());
            droppedEntity->setParent(entity);
            core->getScene()->removeEntity(droppedEntity);
        }
        ImGui::EndDragDropTarget();
    }

    if (open)
    {
        const auto &children = entity->getChildren();

        for (const auto &child : children)
        {
            std::shared_ptr<Entity> childPtr = std::dynamic_pointer_cast<Entity>(child);
            if (childPtr)
            {
                DrawEntityNode(selectedEntity, childPtr);
            }
        }
        ImGui::TreePop();
    }
}

template <typename T>
inline void UIDrawer::DrawInspector(T &obj, const std::string_view &title)
{
    if (!ImGui::TreeNode(title.data()))
        return;

    hana::for_each(hana::accessors<T>(), [&](auto pair)
    {
        constexpr auto name = hana::first(pair);
        auto accessor = hana::second(pair);
        using MemberType = std::remove_reference_t<decltype(accessor(obj))>;
        const std::string_view fieldName = hana::to<const char *>(name);

        if constexpr (std::is_same_v<MemberType, float>) {
            ImGui::DragFloat(fieldName.data(), &accessor(obj), 0.1f);
        } else if constexpr (std::is_same_v<MemberType, int>) {
            ImGui::DragInt(fieldName.data(), &accessor(obj));
        } else if constexpr (std::is_same_v<MemberType, bool>) {
            ImGui::Checkbox(fieldName.data(), &accessor(obj));
        } else if constexpr (std::is_same_v<MemberType, glm::vec3>) {
            ImGui::DragFloat3(fieldName.data(), &accessor(obj)[0], 0.1f);
        } else if constexpr (std::is_same_v<MemberType, glm::quat>) {
            ImGui::DragFloat4(fieldName.data(), &accessor(obj)[0], 0.1f);
        } else if constexpr (std::is_same_v<MemberType, glm::vec4>) {
            ImGui::DragFloat4(fieldName.data(), &accessor(obj)[0], 0.1f);
        } else if constexpr (std::is_same_v<MemberType, std::string>) {
            ImGui::InputText(fieldName.data(), &accessor(obj));
        } else if constexpr (std::is_enum_v<MemberType>) {
            auto names = magic_enum::enum_names<MemberType>();
            std::vector<const char*> items;
            items.reserve(names.size());
            
            // Convertendo cada string_view para const char*
            for (const auto& name : names) {
                items.push_back(name.data());
            }
            
            int currentItem = static_cast<int>(accessor(obj));
            if (ImGui::Combo(fieldName.data(), &currentItem, items.data(), static_cast<int>(items.size()))) {
                accessor(obj) = static_cast<MemberType>(currentItem);
            }
        } else {
            // Para structs aninhadas que também usam BOOST_HANA_DEFINE_STRUCT
            // DrawInspector(accessor(obj), fieldName);
        } 
    });

    ImGui::TreePop();
}
