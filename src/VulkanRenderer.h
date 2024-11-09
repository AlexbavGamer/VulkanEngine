#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "Singleton.h"
#include "Scene.h"
#include "core/VulkanCore.h"

#include "core/VulkanPipeline.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanSwapChain.h"
#include "core/VulkanImGui.h"
class VulkanRenderer : public Singleton<VulkanRenderer> {
    friend class Singleton<VulkanRenderer>;

public:
    VulkanRenderer();
    ~VulkanRenderer();

    void initVulkan(GLFWwindow* window);
    void renderFrame();
    void cleanup();
    void checkWireframeModeChange();

    VulkanCore* getCore() { return core.get(); }
    Scene* getScene() { return scene.get(); }
    uint32_t getCurrentFrame() { return currentFrame; }
private:
    // Core components
    std::unique_ptr<VulkanCore> core;
    std::unique_ptr<Scene> scene;
    
    // Synchronization objects
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    // Command resources
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    // Private methods
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    
    // Constants
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
};

#endif // VULKANRENDERER_H
