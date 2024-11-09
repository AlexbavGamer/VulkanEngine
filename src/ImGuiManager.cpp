#include "ImGuiManager.h"
#include <iostream>

ImGuiManager::ImGuiManager(
    VkInstance instance, 
    VkPhysicalDevice physicalDevice, 
    VkDevice device, 
    VkQueue graphicsQueue, 
    uint32_t graphicsQueueFamily, 
    VkRenderPass renderPass, 
    GLFWwindow* window, 
    VkDescriptorPool descriptorPool
) : 
    m_instance(instance),
    m_physicalDevice(physicalDevice),
    m_device(device),
    m_graphicsQueue(graphicsQueue),
    m_graphicsQueueFamily(graphicsQueueFamily),
    m_renderPass(renderPass),
    m_window(window),
    m_descriptorPool(descriptorPool)
{
IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // Configure ImGui IO
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Set dark theme
    ImGui::StyleColorsDark();

    // Initialize Vulkan backend
    ImGui_ImplGlfw_InitForVulkan(m_window, true);
    
    // Prepare Vulkan initialization info
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_instance;
    init_info.PhysicalDevice = m_physicalDevice;
    init_info.Device = m_device;
    init_info.QueueFamily = m_graphicsQueueFamily;
    init_info.Queue = m_graphicsQueue;
    init_info.DescriptorPool = m_descriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.RenderPass = m_renderPass;
    
    // Initialize ImGui Vulkan backend
    ImGui_ImplVulkan_Init(&init_info);

    // Upload fonts
    uploadFonts();
}

void ImGuiManager::initImGui() {
    // Setup ImGui context
    setupImGuiContext();
    
    // Setup Vulkan-specific ImGui context
    setupImGuiVulkanContext();
    
    // Upload fonts to GPU
    uploadFonts();
}

void ImGuiManager::setupImGuiContext() {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // Configure ImGui IO
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Set dark theme
    ImGui::StyleColorsDark();
}

void ImGuiManager::setupImGuiVulkanContext() {
    // Initialize GLFW for Vulkan
    ImGui_ImplGlfw_InitForVulkan(m_window, true);
    
    // Prepare Vulkan initialization info
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_instance;
    init_info.PhysicalDevice = m_physicalDevice;
    init_info.Device = m_device;
    init_info.QueueFamily = m_graphicsQueueFamily;
    init_info.Queue = m_graphicsQueue;
    init_info.DescriptorPool = m_descriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.RenderPass = m_renderPass;
    
    // Initialize ImGui Vulkan backend
    ImGui_ImplVulkan_Init(&init_info);
}

void ImGuiManager::uploadFonts() {
     // Create temporary command pool
    VkCommandPool command_pool;
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_graphicsQueueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(m_device, &poolInfo, nullptr, &command_pool);

    // Allocate command buffer
    VkCommandBuffer command_buffer;
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(m_device, &allocInfo, &command_buffer);

    // Begin command buffer
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(command_buffer, &begin_info);

    // Create fonts texture
    ImGui_ImplVulkan_CreateFontsTexture();

    // End command buffer
    vkEndCommandBuffer(command_buffer);

    // Submit command buffer
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    // Cleanup
    ImGui_ImplVulkan_DestroyFontsTexture();
    vkFreeCommandBuffers(m_device, command_pool, 1, &command_buffer);
    vkDestroyCommandPool(m_device, command_pool, nullptr);
}

void ImGuiManager::beginFrame() {
  // Ensure any previous frame is ended
    if (ImGui::GetCurrentContext()) {
        ImGui::EndFrame();
    }
    
    // Start new frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::endFrame(VkCommandBuffer commandBuffer) {
    // Render ImGui
    ImGui::Render();
    
    // Get draw data
    ImDrawData* drawData = ImGui::GetDrawData();
    
    // Render draw data if available
    if (drawData) {
        ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
    }
    
    // Update and render platform windows
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    
    // End the frame
    ImGui::EndFrame();
}

void ImGuiManager::drawImGui() 
{
      // Ensure a frame is started
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Ensure the window is always ended, even if an exception occurs
    try {
        ImGui::Begin("Debug Window");
        
        ImGui::Text("Vulkan Renderer Debug");

        static bool wireframeMode = false;
        if (ImGui::Checkbox("Wireframe Mode", &wireframeMode)) {
            
        }

        ImGui::End();  // Ensure this is always called
    }
    catch (const std::exception& e) {
        // If an exception occurs, still end the ImGui window
        ImGui::End();
        throw;  // Rethrow the exception
    }
}

void ImGuiManager::preRecreateSwapChain() {
    // Wait for device to be idle
    vkDeviceWaitIdle(m_device);

    // End any ongoing ImGui frame
    ImGui::Render();
    ImGui::EndFrame();

    // Shutdown Vulkan-specific ImGui implementations
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void ImGuiManager::postRecreateSwapChain(
    VkInstance instance, 
    VkPhysicalDevice physicalDevice, 
    VkDevice device, 
    VkQueue graphicsQueue, 
    uint32_t graphicsQueueFamily, 
    VkRenderPass renderPass, 
    GLFWwindow* window, 
    VkDescriptorPool descriptorPool
) {
    // Update member variables
    m_instance = instance;
    m_physicalDevice = physicalDevice;
    m_device = device;
    m_graphicsQueue = graphicsQueue;
    m_graphicsQueueFamily = graphicsQueueFamily;
    m_renderPass = renderPass;
    m_window = window;
    m_descriptorPool = descriptorPool;

    // Reinitialize ImGui Vulkan context
    setupImGuiVulkanContext();

    // Reinitialize GLFW for Vulkan
    ImGui_ImplGlfw_InitForVulkan(m_window, true);

    // Reupload fonts
    uploadFonts();
}

void ImGuiManager::shutdown() {
// Ensure device is idle
    vkDeviceWaitIdle(m_device);

    // Shutdown ImGui Vulkan and GLFW implementations
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    // Destroy ImGui context
    ImGui::DestroyContext();
}