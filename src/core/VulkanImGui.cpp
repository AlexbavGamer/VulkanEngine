#include "VulkanImGui.h"
#include <stdexcept>
#include "VulkanCore.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptor.h"
#include "VulkanImGui.h"
#include <imgui_internal.h>

#include <iostream>

VulkanImGui::VulkanImGui(VulkanCore& core) : core(core), imguiPool(VK_NULL_HANDLE) {
IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

VulkanImGui::~VulkanImGui() {
    if(ImGui::GetCurrentContext())
    {
        cleanup();
        ImGui::DestroyContext();
    }
}

void VulkanImGui::init(VkRenderPass renderPass) {
    if (imguiPool != VK_NULL_HANDLE) {
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

void VulkanImGui::cleanup() {
    if (imguiPool != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(core.getDevice());
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        vkDestroyDescriptorPool(core.getDevice(), imguiPool, nullptr);
        imguiPool = VK_NULL_HANDLE;
    }
}

void VulkanImGui::render(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set Unity-like dark theme
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);

    // Add spacing between windows
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.WindowBorderSize = 1.0f;
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);

    // Main dockspace setup
    ImGuiViewport* viewport = ImGui::GetMainViewport();
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

    // Menu Bar
    static bool showNewScene = false;
    static bool showOpenScene = false;
    static bool showSaveScene = false;
    static bool showSaveSceneAs = false;
    static bool showImportAsset = false;
    static bool showExportScene = false;
    static bool showBuildSettings = false;
    static bool showProjectSettings = false;
    static bool showStatistics = false;
    static bool showDebugWindow = false;
    static bool showDocumentation = false;
    static bool showApiReference = false;
    static bool showReleaseNotes = false;
    static bool showBugReport = false;
    static bool showFeatureRequest = false;
    static bool showAbout = false;


    static bool showScene = true;
    static bool showGame = false;
    static bool showHierarchy = true;
    static bool showInspector = true;
    static bool showProject = true;
    static bool showConsole = true;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) { showNewScene = true; }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) { showOpenScene = true; }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) { showSaveScene = true; }
            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) { showSaveSceneAs = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("Import Asset", "Ctrl+I")) { showImportAsset = true; }
            if (ImGui::MenuItem("Export Scene", "Ctrl+E")) { showExportScene = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("Build Settings", "Ctrl+B")) { showBuildSettings = true; }
            if (ImGui::MenuItem("Project Settings", "Ctrl+,")) { showProjectSettings = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
            if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {}
            if (ImGui::MenuItem("Delete", "Del")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "Ctrl+A")) {}
            if (ImGui::MenuItem("Deselect All", "Ctrl+Shift+A")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Play", "Ctrl+P")) {}
            if (ImGui::MenuItem("Pause", "Ctrl+Shift+P")) {}
            if (ImGui::MenuItem("Stop", "Ctrl+T")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
    
            
            if (ImGui::MenuItem("Scene", nullptr, &showScene)) {}
            if (ImGui::MenuItem("Game", nullptr, &showGame)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy)) {}
            if (ImGui::MenuItem("Inspector", nullptr, &showInspector)) {}
            if (ImGui::MenuItem("Project", nullptr, &showProject)) {}
            if (ImGui::MenuItem("Console", nullptr, &showConsole)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Statistics", nullptr, &showStatistics)) {}
            if (ImGui::MenuItem("Debug", nullptr, &showDebugWindow)) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            static bool isMaximized = false;
            static bool isFullscreen = false;
            if (ImGui::MenuItem("Maximize", "Alt+Enter", &isMaximized)) {}
            if (ImGui::MenuItem("Fullscreen", "F11", &isFullscreen)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout")) {}
            if (ImGui::MenuItem("Save Layout")) {}
            if (ImGui::MenuItem("Load Layout")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Documentation")) { showDocumentation = true; }
            if (ImGui::MenuItem("API Reference")) { showApiReference = true; }
            if (ImGui::MenuItem("Release Notes")) { showReleaseNotes = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("Report Bug")) { showBugReport = true; }
            if (ImGui::MenuItem("Feature Request")) { showFeatureRequest = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("About")) { showAbout = true; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id);

    // Set initial dock layout
    static const float MIN_PANEL_WIDTH = 250.0f;
    static bool first_time = true;
    if (first_time) {
        first_time = false;
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, MIN_PANEL_WIDTH / viewport->Size.x, nullptr, &dock_main_id);
        ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, MIN_PANEL_WIDTH / (viewport->Size.x - MIN_PANEL_WIDTH), nullptr, &dock_main_id);

        ImGui::DockBuilderDockWindow("Scene", dock_main_id);
        ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    // Windows with maximum width constraints
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene", &showScene, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)sceneDescriptorSet, viewportSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1));
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, -1.0f), ImVec2(350.0f, -1.0f));
    ImGui::Begin("Inspector", &showInspector);
    static bool wireframeMode = false;
    if(ImGui::Checkbox("Wireframe Mode", &wireframeMode)) {
        core.getPipeline()->setWireframeMode(wireframeMode);
    }
    ImGui::End();

    ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, -1.0f), ImVec2(350.0f, -1.0f));
    ImGui::Begin("Hierarchy", &showHierarchy);
    ImGui::Text("Scene Objects");
    ImGui::End();

    // Render additional windows based on menu selections
    if (showStatistics) {
        ImGui::Begin("Statistics", &showStatistics);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::End();
    }

    if (showDebugWindow) {
        ImGui::Begin("Debug", &showDebugWindow);
        ImGui::Text("Debug Information");
        ImGui::End();
    }

    ImGui::End(); // DockSpace

    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    if (drawData) {
        ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
    }
}

void VulkanImGui::setupDescriptorPool() {
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(core.getDevice(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create ImGui descriptor pool!");
    }
}