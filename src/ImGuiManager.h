#pragma once

#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

class VulkanRenderer;

class ImGuiManager {
public:
    // Constructor with comprehensive Vulkan initialization parameters
    ImGuiManager(
        VkInstance instance, 
        VkPhysicalDevice physicalDevice, 
        VkDevice device, 
        VkQueue graphicsQueue, 
        uint32_t graphicsQueueFamily, 
        VkRenderPass renderPass, 
        GLFWwindow* window, 
        VkDescriptorPool descriptorPool
    );
    
    // Initialize ImGui systems
    void initImGui();
    
    // Begin a new ImGui frame
    void beginFrame();
    
    // End ImGui frame and render
    void endFrame(VkCommandBuffer commandBuffer);
    
    // Shutdown ImGui systems
    void shutdown();

    // Draw debug window
    void drawImGui();

    // Prepare for swap chain recreation
    void preRecreateSwapChain();

    // Reinitialize after swap chain recreation
    void postRecreateSwapChain(
        VkInstance instance, 
        VkPhysicalDevice physicalDevice, 
        VkDevice device, 
        VkQueue graphicsQueue, 
        uint32_t graphicsQueueFamily, 
        VkRenderPass renderPass, 
        GLFWwindow* window, 
        VkDescriptorPool descriptorPool
    );

private:
    // Vulkan-related members
    VkInstance m_instance;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    uint32_t m_graphicsQueueFamily;
    VkRenderPass m_renderPass;
    GLFWwindow* m_window;
    VkDescriptorPool m_descriptorPool;

    // Setup ImGui context
    void setupImGuiContext();

    // Setup Vulkan-specific ImGui context
    void setupImGuiVulkanContext();

    // Upload fonts to GPU
    void uploadFonts();
};

#endif // IMGUI_MANAGER_H
