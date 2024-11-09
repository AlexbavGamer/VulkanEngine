#include "VulkanImGui.h"
#include <stdexcept>
#include "VulkanCore.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptor.h"
#include "VulkanImGui.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

VulkanImGui::VulkanImGui(VulkanCore* core) : core(core), imguiPool(VK_NULL_HANDLE) {}

VulkanImGui::~VulkanImGui() {
    cleanup();
}

void VulkanImGui::init(VkRenderPass renderPass) {
    setupDescriptorPool();
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

void VulkanImGui::cleanup() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (imguiPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(core->getDevice(), imguiPool, nullptr);
        imguiPool = VK_NULL_HANDLE;
    }
}

void VulkanImGui::newFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void VulkanImGui::render(VkCommandBuffer commandBuffer) {
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    if (drawData && drawData->CmdListsCount > 0) {
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

    if (vkCreateDescriptorPool(core->getDevice(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create ImGui descriptor pool!");
    }
}