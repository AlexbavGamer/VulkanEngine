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

#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"

#include "VulkanRenderer.h"

#include "Roboto-Regular.h"
#include "fa-solid-900.h"
#include <ui/UIdrawer.h>

VulkanImGui::VulkanImGui(VulkanCore *core) : core(core), vulkanRenderer(VulkanRenderer::getInstance()), imguiPool(VK_NULL_HANDLE)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    ImFont *robotoFont = io.Fonts->AddFontFromMemoryTTF((void *)EmbeddedFonts::Roboto_Regular, sizeof(EmbeddedFonts::Roboto_Regular), 20.0f, &fontConfig);
    io.FontDefault = robotoFont;

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    iconFont = io.Fonts->AddFontFromMemoryTTF((void *)EmbeddedFonts::fa_solid_900, sizeof(EmbeddedFonts::fa_solid_900), 16.0f, &icons_config, icons_ranges);

    this->drawer = new UIDrawer(core);

    setupStyles();
}

VulkanImGui::~VulkanImGui()
{
    if (ImGui::GetCurrentContext())
    {
        cleanup();
    }
}

void VulkanImGui::init(VkRenderPass renderPass)
{
    if (imguiPool != VK_NULL_HANDLE)
    {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(core->getDevice(), imguiPool, nullptr);
        imguiPool = VK_NULL_HANDLE;
    }

    setupDescriptorPool();

    ImGui_ImplGlfw_InitForVulkan(core->getWindow(), true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = core->getInstance();
    init_info.PhysicalDevice = core->getPhysicalDevice();
    init_info.Device = core->getDevice();
    init_info.QueueFamily = core->getQueueFamilyIndex();
    init_info.Queue = core->getGraphicsQueue();
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
    // style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
    // style.Colors[ImGuiCol_WindowBg] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    // style.Colors[ImGuiCol_Header] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    // style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    // style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    // style.Colors[ImGuiCol_Button] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    // style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    // style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    // style.Colors[ImGuiCol_FrameBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    // style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    // style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    // style.Colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    // style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    // style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    // style.Colors[ImGuiCol_PopupBg] = ImVec4(0.23f, 0.23f, 0.23f, 0.94f);
    // style.Colors[ImGuiCol_Border] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);

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
        vkDeviceWaitIdle(core->getDevice());
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        vkDestroyDescriptorPool(core->getDevice(), imguiPool, nullptr);
        imguiPool = VK_NULL_HANDLE;
    }
}

void VulkanImGui::render(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Setup dockspace
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
    {
        ImGui::PopStyleVar(3);

        drawer->drawMainMenuBar();
        ImGuiID dockspace_id = ImGui::GetID("EngineDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

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

        drawer->drawSceneWindow(sceneDescriptorSet);
        drawer->drawInspectorWindow(selectedEntity);
        drawer->drawHierarchyWindow(selectedEntity);
        drawer->drawContentBrowser();
    }
    ImGui::End();

    // Render modals after the DockSpace
    drawer->drawProjectCreationModal();
    drawer->drawOpenProjectModal();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
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

    if (vkCreateDescriptorPool(core->getDevice(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
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