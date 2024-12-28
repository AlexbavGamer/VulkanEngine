#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <memory>
#include <array>

#include "VulkanTypes.h"

class VulkanSwapChain;
class VulkanPipeline;
class VulkanDescriptor;
class VulkanImGui;
class ProjectManager;
class Scene;

class VulkanCore {
public:
    void init(GLFWwindow* window);
    void cleanup();
    void renderFrame();
    void handleResize();
    void waitIdle() { vkDeviceWaitIdle(device); }

    // Getters
    VkDevice getDevice() const { return device; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    VkCommandPool getCommandPool() const { return commandPool; }
    VkRenderPass getRenderPass() const { return renderPass; }
    VkRenderPass getSceneRenderPass() const { return sceneRenderPass; }
    VulkanSwapChain* getSwapChain() const { return swapChain.get(); }
    VulkanPipeline* getPipeline() const { return pipeline.get(); }
    VulkanDescriptor* getDescriptor() const { return descriptor.get(); }
    Scene* getScene() const { return scene.get(); }
    uint32_t getQueueFamilyIndex();
    GLFWwindow* getWindow() const { return window; }
    VkInstance getInstance() const { return instance; }
    VkSurfaceKHR getSurface() const { return surface; }
    VkImageView getDepthImageView() const { return depthImageView; }
    uint32_t getMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }
    VkDescriptorSetLayout getSceneDescriptorSetLayout() const { return sceneDescriptorSetLayout; }
    VkDescriptorSet getSceneDescriptorSet() const { return sceneDescriptorSet; }
    VkBuffer getSceneUniformBuffer() const { return sceneUniformBuffer; }
    ProjectManager* getProjectManager() const;
    // Resource Creation
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void createImage(uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage &image,
                     VkDeviceMemory &imageMemory);
    VkSampler createTextureSampler(VkSamplerCreateInfo& samplerInfo);

    // Command Buffer Methods
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void beginCommandBuffer(VkCommandBuffer commandBuffer);
    void endCommandBuffer(VkCommandBuffer commandBuffer);
    void beginRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                        const VkClearValue* clearValues, uint32_t clearValueCount);

    // Memory Management
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkBuffer& buffer,
                     VkDeviceMemory& bufferMemory);
    void copyDataToBuffer(const void* data, VkDeviceMemory bufferMemory, VkDeviceSize size);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void transitionImageLayout(VkImage image, VkFormat format, 
                             VkImageLayout oldLayout, VkImageLayout newLayout);

    static std::vector<char> readFile(const std::string& filename);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    void validateImageDimensions(uint32_t &width, uint32_t &height);
    void createRenderPass();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    GLFWwindow* window;
    VkInstance instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};
    VkQueue graphicsQueue{VK_NULL_HANDLE};
    VkQueue presentQueue{VK_NULL_HANDLE};
    VkSurfaceKHR surface{VK_NULL_HANDLE};
    VkDescriptorSetLayout sceneDescriptorSetLayout{VK_NULL_HANDLE};
    VkCommandPool commandPool{VK_NULL_HANDLE};
    VkRenderPass renderPass{VK_NULL_HANDLE};
    VkSampler textureSampler{VK_NULL_HANDLE};

    std::unique_ptr<VulkanSwapChain> swapChain;
    std::unique_ptr<VulkanPipeline> pipeline;
    std::unique_ptr<VulkanDescriptor> descriptor;
    std::unique_ptr<VulkanImGui> imgui;
    std::unique_ptr<Scene> scene;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFramebuffer> framebuffers;

    uint32_t currentFrame = 0;

    // Scene Resources
    VkImage sceneImage{VK_NULL_HANDLE};
    VkDeviceMemory sceneImageMemory{VK_NULL_HANDLE};
    VkImageView sceneImageView{VK_NULL_HANDLE};
    VkSampler sceneSampler{VK_NULL_HANDLE};
    VkRenderPass sceneRenderPass{VK_NULL_HANDLE};
    VkFramebuffer sceneFramebuffer{VK_NULL_HANDLE};
    VkDescriptorSet sceneDescriptorSet{VK_NULL_HANDLE};
    
    VkBuffer sceneUniformBuffer{VK_NULL_HANDLE};
    VkDeviceMemory sceneUniformBufferMemory{VK_NULL_HANDLE};

    // Depth Resources
    VkImage depthImage{VK_NULL_HANDLE};
    VkDeviceMemory depthImageMemory{VK_NULL_HANDLE};
    VkImageView depthImageView{VK_NULL_HANDLE};

    // Default Texture Resources
    VkImage defaultTexture{VK_NULL_HANDLE};
    VkDeviceMemory defaultTextureMemory{VK_NULL_HANDLE};
    VkImageView defaultTextureView{VK_NULL_HANDLE};

    // Initialization Methods
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void createFramebuffers();
    void createDepthResources();
    void createTextureSampler();
    void createSceneFramebuffer();
    void createSceneResources();
    void createSceneRenderPass();

    // Helper Methods
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkValidationLayerSupport();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool hasStencilComponent(VkFormat format);
    std::vector<const char*> getRequiredExtensions();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                                VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
};