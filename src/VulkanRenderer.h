#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "TextureManager.h"
#include "ShaderManager.h"
#include "RenderPass.h"
#include "DescriptorCache.h"
#include "ImGuiManager.h"

#include <atomic>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include "scene.h"
#include "Singleton.h"

// Estruturas auxiliares
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct ThreadResources {
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
extern std::vector<ThreadResources> threadResources;

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer : public Singleton<VulkanRenderer> {
    friend class Singleton<VulkanRenderer>;

public:
    // Construtores e destrutores
    VulkanRenderer();
    ~VulkanRenderer();

    // Métodos principais
    void initVulkan(GLFWwindow* window);
    void renderFrame();
    void cleanup();

    // Getters públicos
    VkInstance getVkInstance() const { return instance; }
    VkDebugUtilsMessengerEXT getDebugMessenger() const { return debugMessenger; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkDevice getDevice() const { return device; }
    VkSurfaceKHR getSurface() const { return surface; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    VkSwapchainKHR getSwapChain() const { return swapChain; }
    VkExtent2D getSwapChainExtent() const { return swapChainExtent; }
    VkFormat getSwapChainImageFormat() const { return swapChainImageFormat; }
    const std::vector<VkImage>& getSwapChainImages() const { return swapChainImages; }
    const std::vector<VkImageView>& getSwapChainImageViews() const { return swapChainImageViews; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }
    VkRenderPass getRenderPass() const { return renderPass; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
    VkPipeline getGraphicsPipeline() const { return graphicsPipeline; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
    VkDescriptorSet getDescriptorSet() const { return descriptorSet; }
    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return descriptorSets; }
    uint32_t getCurrentFrame() const { return currentFrame; }

    // Métodos públicos de utilidade
    void createEntityDescriptorPool(MaterialComponent &material);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void checkWireframeModeChange();
    void createUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void updateUniformBuffer(VkCommandBuffer commandBuffer, VkBuffer uniformBuffer, const UBO& ubo);
    void copyDataToBuffer(const void* data, VkDeviceMemory bufferMemory, VkDeviceSize size);
    void createGraphicsPipeline();

    void setWireframeMode(bool enabled) 
    { 
        vkDeviceWaitIdle(device);
    
        // Reset command buffers
        for (auto& commandBuffer : commandBuffers) {
            vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        }
        
        m_wireframeMode = enabled;
        
        // Store old pipeline handles
        VkPipeline oldPipeline = graphicsPipeline;
        VkPipelineLayout oldLayout = pipelineLayout;
        
        // Set handles to null before recreation
        graphicsPipeline = VK_NULL_HANDLE;
        pipelineLayout = VK_NULL_HANDLE;
        
        // Recreate pipeline and dependent resources
        cleanupSwapChain();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createDescriptorPool();
        createDescriptorSets();
        
        // Clean up old resources after new ones are created
        if (oldPipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(device, oldPipeline, nullptr);
        }
        if (oldLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(device, oldLayout, nullptr);
        }
    }

    // Membros públicos
    std::unique_ptr<Scene> scene;
    VkShaderModule vertShaderModule = VK_NULL_HANDLE;
    VkShaderModule fragShaderModule = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;

private:
    // Constantes
    const uint32_t MAX_FRAMES_IN_FLIGHT = 1;
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    // Métodos privados - Inicialização
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void createDescriptorPool();
    void createDescriptorSets();
    void createDepthResources();
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void cleanupImGui();
    void setupImGuiDescriptorPool();
    void initImGui();

    VkFormat findDepthFormat();
    
    // Métodos privados - Suporte
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);


    // Métodos privados - Limpeza e recriação
    void cleanupSwapChain();
    void recreateSwapChain();
    void recreateGraphicsPipeline();
    void cleanupSwapChainResources();

    // Métodos privados - Renderização
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    // Métodos privados - Utilitários
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    // Membros privados - Handles principais
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    // Membros privados - Swapchain
    VkSwapchainKHR swapChain;
    VkExtent2D swapChainExtent;
    VkFormat swapChainImageFormat;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> framebuffers;

    // Membros privados - Descritores
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    std::vector<VkDescriptorSet> descriptorSets;

    // Membros privados - Comandos
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    // Membros privados - Buffers
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    // Membros privados - Sincronização
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    // Membros privados - Imgui
    std::unique_ptr<ImGuiManager> imguiManager;
    VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;

    std::atomic<bool> requestedWireframe{false};
    bool m_wireframeMode = false;
    std::mutex pipelineMutex;
};

#endif // VULKANRENDERER_H
