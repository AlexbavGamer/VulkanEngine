#include "VulkanImGui.h"
#include <stdexcept>
#include "VulkanCore.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptor.h"
#include "VulkanImGui.h"
#include <imgui_internal.h>

#include <typeinfo.h>
#include "../Scene.h"
#include <iostream>
#include <variant>
#include <managers/FileManager.h>

VulkanImGui::VulkanImGui(VulkanCore &core) : core(core), imguiPool(VK_NULL_HANDLE)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    float baseFontSize = 16.0f;
    io.Fonts->AddFontDefault();

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    iconFont = io.Fonts->AddFontFromFileTTF("engine/fonts/fa-solid-900.ttf", baseFontSize, &icons_config, icons_ranges);

    setupStyles();
}

VulkanImGui::~VulkanImGui()
{
    if (ImGui::GetCurrentContext())
    {
        cleanup();
        ImGui::DestroyContext();
    }
}

void VulkanImGui::init(VkRenderPass renderPass)
{
    if (imguiPool != VK_NULL_HANDLE)
    {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(core.getDevice(), imguiPool, nullptr);
        imguiPool = VK_NULL_HANDLE;
    }

    setupDescriptorPool();

    ImGui_ImplGlfw_InitForVulkan(core.getWindow(), true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = core.getInstance();
    init_info.PhysicalDevice = core.getPhysicalDevice();
    init_info.Device = core.getDevice();
    init_info.QueueFamily = core.getQueueFamilyIndex();
    init_info.Queue = core.getGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;
    init_info.RenderPass = renderPass;

    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();
}

void VulkanImGui::setupStyles()
{
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();

    // Unity 5 Dark Theme colors
    style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.23f, 0.23f, 0.23f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);

    // Unity-like spacing and padding
    style.WindowMinSize = ImVec2(1.0f, 1.0f);
    style.WindowPadding = ImVec2(0.0f, 0.0f);
    style.DisplaySafeAreaPadding = ImVec2(0.0f, 0.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.WindowBorderSize = 1.0f;
    style.WindowRounding = 2.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 2.0f;
}

void VulkanImGui::cleanup()
{
    if (imguiPool != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(core.getDevice());
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        vkDestroyDescriptorPool(core.getDevice(), imguiPool, nullptr);
        imguiPool = VK_NULL_HANDLE;
    }
}

void VulkanImGui::render(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Main dockspace setup
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // File menu dialogs
    static bool showNewScene = false;
    static bool showOpenScene = false;
    static bool showSaveScene = false;
    static bool showSaveSceneAs = false;
    static bool showImportAsset = false;
    static bool showExportScene = false;
    static bool showBuildSettings = false;
    static bool showProjectSettings = false;

    // -
    static bool showStatistics = false;
    static bool showDebugWindow = false;

    // Main window visibility states
    static bool showScene = true;
    static bool showGame = false;
    static bool showHierarchy = true;
    static bool showInspector = true;
    static bool showProject = true;
    static bool showConsole = true;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene", "Ctrl+N"))
            {
                showNewScene = true;
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
            {
                showOpenScene = true;
            }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                showSaveScene = true;
            }
            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
            {
                showSaveSceneAs = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Import Asset", "Ctrl+I"))
            {
                showImportAsset = true;
            }
            if (ImGui::MenuItem("Export Scene", "Ctrl+E"))
            {
                showExportScene = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Build Settings", "Ctrl+B"))
            {
                showBuildSettings = true;
            }
            if (ImGui::MenuItem("Project Settings", "Ctrl+,"))
            {
                showProjectSettings = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X"))
            {
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V"))
            {
            }
            if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
            {
            }
            if (ImGui::MenuItem("Delete", "Del"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "Ctrl+A"))
            {
            }
            if (ImGui::MenuItem("Deselect All", "Ctrl+Shift+A"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Play", "Ctrl+P"))
            {
            }
            if (ImGui::MenuItem("Pause", "Ctrl+Shift+P"))
            {
            }
            if (ImGui::MenuItem("Stop", "Ctrl+T"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Scene", nullptr, &showScene))
            {
            }
            if (ImGui::MenuItem("Game", nullptr, &showGame))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy))
            {
            }
            if (ImGui::MenuItem("Inspector", nullptr, &showInspector))
            {
            }
            if (ImGui::MenuItem("Project", nullptr, &showProject))
            {
            }
            if (ImGui::MenuItem("Console", nullptr, &showConsole))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Statistics", nullptr, &showStatistics))
            {
            }
            if (ImGui::MenuItem("Debug", nullptr, &showDebugWindow))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            static bool isMaximized = false;
            static bool isFullscreen = false;
            if (ImGui::MenuItem("Maximize", "Alt+Enter", &isMaximized))
            {
            }
            if (ImGui::MenuItem("Fullscreen", "F11", &isFullscreen))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout"))
            {
            }
            if (ImGui::MenuItem("Save Layout"))
            {
            }
            if (ImGui::MenuItem("Load Layout"))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGuiID dockspace_id = ImGui::GetID("EngineDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    // Set initial dock layout
    static const float MIN_PANEL_WIDTH = 250.0f;
    static bool first_time = true;
    if (first_time)
    {
        first_time = false;
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, MIN_PANEL_WIDTH / viewport->Size.x, nullptr, &dock_main_id);
        ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, MIN_PANEL_WIDTH / (viewport->Size.x - MIN_PANEL_WIDTH), nullptr, &dock_main_id);
        ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);

        ImGui::DockBuilderDockWindow("Scene", dock_main_id);
        ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
        ImGui::DockBuilderDockWindow("Content Browser", dock_bottom_id);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    // Windows with maximum width constraints
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene", &showScene,
                 ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoNavFocus |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)sceneDescriptorSet,
                 viewportSize,
                 ImVec2(0, 0), ImVec2(1, 1),
                 ImVec4(1, 1, 1, 1));

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, -1.0f), ImVec2(350.0f, -1.0f));
    ImGui::Begin("Inspector", &showInspector);
    if (selectedEntity)
    {
        for (const auto &[type_index, component] : selectedEntity->getComponents())
        {
            if (component)
            { // Check if component is valid
                component->renderComponent();
            }
        }
    }
    ImGui::End();

    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, -1.0f), ImVec2(350.0f, -1.0f));
    ImGui::Begin("Hierarchy", &showHierarchy);

    core.getScene()->registry->view<RenderComponent>([&](std::shared_ptr<Entity> entity, RenderComponent &render)
                                                     {
        if (ImGui::Selectable(render.name.c_str(), selectedEntity && selectedEntity->getId() == entity->getId())) 
        {
            if (selectedEntity && selectedEntity->getId() == entity->getId()) {
                selectedEntity = nullptr;
            } else {
                selectedEntity = entity;
            }
        } });

    ImGui::End();

    // Content Browser

    ImGui::Begin("Content Browser", nullptr);
    {
        static std::string currentPath = "Assets/";
        auto &fileManager = FileManager::getInstance();

        // Path navigation bar
        ImGui::Text(currentPath.c_str());

        // Calculate grid layout
        float padding = 16.0f;
        float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = std::max(1, (int)(panelWidth / cellSize));

        ImGui::Separator();

        // Grid layout
        if (ImGui::BeginTable("FileGrid", columnCount))
        {
            auto files = fileManager.listFiles(currentPath);
            std::vector<std::string> directories;
            std::vector<std::string> regularFiles;

            // Sort into directories and files
            for (const auto &file : files)
            {
                if (std::filesystem::is_directory(currentPath + file))
                {
                    directories.push_back(file);
                }
                else
                {
                    regularFiles.push_back(file);
                }
            }

            // Display back button if not in root directory
            if (currentPath != "Assets/")
            {
                ImGui::TableNextColumn();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

                if (ImGui::Button((std::string(ICON_FA_ARROW_LEFT) + "##back").c_str(), ImVec2(thumbnailSize, thumbnailSize)))
                {
                    size_t lastSlash = currentPath.find_last_of("/", currentPath.length() - 2);
                    if (lastSlash != std::string::npos)
                    {
                        currentPath = currentPath.substr(0, lastSlash + 1);
                    }
                }

                ImGui::PopStyleColor();

                float textWidth = ImGui::CalcTextSize("Back").x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (thumbnailSize - textWidth) * 0.5f);
                ImGui::TextWrapped("Back");
            }

            // Display directories first
            for (const auto &dir : directories)
            {
                ImGui::TableNextColumn();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

                std::string buttonId = "##" + dir;
                if (ImGui::Button((std::string(ICON_FA_FOLDER) + buttonId).c_str(), ImVec2(thumbnailSize, thumbnailSize)))
                {
                    currentPath += dir + "/";
                    ImGui::PopStyleColor();
                    break;
                }

                showFileContextMenu(currentPath + dir, "##folder_" + dir, true);

                if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                {
                    ImGui::OpenPopup(("##folder_" + dir).c_str());
                }

                ImGui::PopStyleColor();

                float textWidth = ImGui::CalcTextSize(dir.c_str()).x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (thumbnailSize - textWidth) * 0.5f);
                ImGui::TextWrapped(dir.c_str());
            }

            // Then display files
            for (const auto &file : regularFiles)
            {
                ImGui::TableNextColumn();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

                std::string buttonId = "##" + file;
                if (ImGui::Button((std::string(ICON_FA_FILE) + buttonId).c_str(), ImVec2(thumbnailSize, thumbnailSize)))
                {
                    handleFileSelection(currentPath + file);
                }

                showFileContextMenu(currentPath + file, "##file_" + file, true);

                if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                {
                    ImGui::OpenPopup(("##file_" + file).c_str());
                }

                ImGui::PopStyleColor();

                float textWidth = ImGui::CalcTextSize(file.c_str()).x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (thumbnailSize - textWidth) * 0.5f);
                ImGui::TextWrapped(file.c_str());
            }

            ImGui::EndTable();
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsAnyItemHovered())
        {
            ImGui::OpenPopup("EmptySpaceContextMenu");
        }

        if (ImGui::BeginPopup("EmptySpaceContextMenu"))
        {
            if (ImGui::MenuItem("Create Folder"))
            {
                std::string newFolderPath = currentPath + "New Folder";
                int counter = 1;
                while (std::filesystem::exists(newFolderPath))
                {
                    newFolderPath = currentPath + "New Folder (" + std::to_string(counter) + ")";
                    counter++;
                }
                std::filesystem::create_directory(newFolderPath);
            }

            if (ImGui::MenuItem("Create File"))
            {
                ImGui::CloseCurrentPopup();
                ImGui::OpenPopup("Create File");
            }

            if (ImGui::MenuItem("Import Asset"))
            {
                // Handle import asset
            }
            if (ImGui::MenuItem("Refresh"))
            {
                // Handle refresh
            }
            ImGui::EndPopup();
        }

        // File creation modal
        static char fileName[256] = "";
        if (ImGui::BeginPopupModal("Create File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));

            if (ImGui::Button("Create"))
            {
                if (strlen(fileName) > 0)
                {
                    std::string newFilePath = currentPath + fileName;

                    // Verificar se o arquivo já existe antes de criar
                    if (!std::filesystem::exists(newFilePath))
                    {
                        std::ofstream file(newFilePath);
                        file.close();
                        ImGui::CloseCurrentPopup();
                        memset(fileName, 0, sizeof(fileName)); // Limpar nome do arquivo
                    }
                    else
                    {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "File already exists!");
                    }
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
                memset(fileName, 0, sizeof(fileName));
            }

            ImGui::EndPopup();
        }

        ImGui::End();
    }

    // Render additional windows based on menu selections    if (showStatistics)
    ImGui::Begin("Statistics", &showStatistics);
    {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    }
    ImGui::End();

    if (showDebugWindow)
    {
        ImGui::Begin("Debug", &showDebugWindow);
        ImGui::Text("Debug Information");

        static bool wireframeMode = false;
        if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
        {
            core.getPipeline()->setWireframeMode(wireframeMode);
        }

        ImGui::End();
    }
    ImGui::End(); // DockSpace

    ImGui::Render();
    ImDrawData *drawData = ImGui::GetDrawData();
    if (drawData)
    {
        ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
    }
}

void VulkanImGui::setupDescriptorPool()
{
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(core.getDevice(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create ImGui descriptor pool!");
    }
}

void VulkanImGui::handleFileSelection(const std::string &filename)
{
    std::string extension = FileManager::getInstance().getFileExtension(filename);

    if (extension == ".txt")
    {
        std::string command = "notepad \"" + filename + "\"";
        system(command.c_str());
    }
}

void VulkanImGui::showFileContextMenu(const std::string &filePath, const std::string id, bool isDirectory)
{
    if (ImGui::BeginPopupContextItem(id.c_str()))
    {
        if (ImGui::MenuItem("Delete"))
        {
            // TODO: Implement file deletion
            std::filesystem::remove(filePath);
        }

        if (!isDirectory)
        {
            if (ImGui::MenuItem("Rename"))
            {
                // TODO: Implement file renaming
            }

            if (ImGui::MenuItem("Open"))
            {
                handleFileSelection(filePath);
            }

            if (ImGui::MenuItem("Copy Path"))
            {
                ImGui::SetClipboardText(filePath.c_str());
            }
        }
        else
        {
            if (ImGui::MenuItem("New File"))
            {
                // TODO: Implement new file creation
            }

            if (ImGui::MenuItem("New Folder"))
            {
                // TODO: Implement new folder creation
            }
        }

        ImGui::EndPopup();
    }
}