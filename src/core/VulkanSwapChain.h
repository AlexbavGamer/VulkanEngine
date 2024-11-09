#pragma once
#include "VulkanTypes.h"

class VulkanCore;

class VulkanSwapChain {
public:
    VulkanSwapChain(VulkanCore* core);
    ~VulkanSwapChain();
    
    void create();
    void cleanup();
    void recreate();
    
    VkSwapchainKHR getSwapChain() const { return swapChain; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkFormat getImageFormat() const { return swapChainImageFormat; }
    const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }
    
private:
    void createSwapChain();
    void createImageViews();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    
    VulkanCore* core;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};