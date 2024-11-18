#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include <string>
#include <memory>

#include "VulkanTypes.h"

class VulkanSwapChain;
class VulkanPipeline;
class VulkanDescriptor;
class VulkanImGui;
class Scene;

class VulkanCore {
public:
    void init(GLFWwindow* window);
    void cleanup();

    void renderFrame();


    VkInstance getInstance() const { return instance; }
    VkDevice getDevice() const { return device; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    VkSurfaceKHR getSurface() const { return surface; }
    GLFWwindow* getWindow() const { return window; }
    uint32_t getQueueFamilyIndex();
    uint32_t getMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }    
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    std::vector<char> readFile(const std::string &filename);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void copyDataToBuffer(const void *data, VkDeviceMemory bufferMemory, VkDeviceSize size);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }
    VkRenderPass getRenderPass() const { return renderPass; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

    VulkanSwapChain* getSwapChain() const { return swapChain.get(); }
    VulkanPipeline* getPipeline() const { return pipeline.get(); }
    VulkanDescriptor* getDescriptor() const { return descriptor.get(); }
    VulkanImGui* getImGui() const { return imgui.get(); }
    Scene* getScene() const { return scene.get(); }

    uint32_t getCurrentFrame() const { return currentFrame; }

    void setPipeline(std::unique_ptr<VulkanPipeline> newPipeline) { pipeline = std::move(newPipeline); }
    void setDescriptor(std::unique_ptr<VulkanDescriptor> newDescriptor) { descriptor = std::move(newDescriptor); }
    void setSwapChain(std::unique_ptr<VulkanSwapChain> newSwapChain) { swapChain = std::move(newSwapChain); }
    void handleResize();
    
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
private:
    const uint32_t MAX_FRAMES_IN_FLIGHT = 1;
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void createSceneResources();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);


    bool checkValidationLayerSupport();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();

    bool isDeviceSuitable(VkPhysicalDevice device);

    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    GLFWwindow* window;

    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;

    std::unique_ptr<VulkanSwapChain> swapChain;
    std::unique_ptr<VulkanPipeline> pipeline;
    std::unique_ptr<VulkanDescriptor> descriptor;
    std::unique_ptr<VulkanImGui> imgui;
    std::unique_ptr<Scene> scene;

    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData); 

private:
    VkImage sceneImage;
    VkDeviceMemory sceneImageMemory;
    VkImageView sceneImageView;
    VkSampler sceneSampler;
    VkFramebuffer sceneFramebuffer;
    VkRenderPass sceneRenderPass;
    VkDescriptorSet sceneDescriptorSet;  
};