#include "VulkanImGui.h"
#include "VulkanCore.h"
#include "VulkanDescriptor.h"
#include "VulkanSwapChain.h"
#include <stdexcept>
#include <array>

#include "Roboto-Regular.h"
#include "fa-solid-900.h"
#include "ui/UIDrawer.h"

void VulkanImGui::init()
{
    // ImGui Initialization for Vulkan
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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

    setupStyles();

    ImGui_ImplGlfw_InitForVulkan(core->getWindow(), true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = core->getInstance();
    init_info.PhysicalDevice = core->getPhysicalDevice();
    init_info.Device = core->getDevice();
    init_info.QueueFamily = core->getQueueFamilyIndex();
    init_info.Queue = core->getGraphicsQueue();
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = core->getDescriptor()->getDescriptorPool();
    init_info.MinImageCount = 2;
    init_info.ImageCount = core->getSwapChain()->getImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.RenderPass = core->getRenderPass();

    ImGui_ImplVulkan_Init(&init_info);

    // Upload Fonts
    VkCommandBuffer commandBuffer = core->beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture();
    core->endSingleTimeCommands(commandBuffer);
    ImGui_ImplVulkan_DestroyFontsTexture();
}

void VulkanImGui::setupStyles()
{
}

void VulkanImGui::cleanup()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void VulkanImGui::render(VkCommandBuffer commandBuffer)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render a window with the scene texture
    ImGui::Begin("Rendered Scene");

    ImTextureID sceneTexture = reinterpret_cast<ImTextureID>(core->getSceneDescriptorSet());
    ImVec2 textureSize = ImVec2(core->getSwapChain()->getExtent().width,
                                core->getSwapChain()->getExtent().height);

    ImGui::Image(sceneTexture, textureSize);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}