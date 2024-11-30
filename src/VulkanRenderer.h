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

    VulkanCore* getCore() { return core.get(); }
private:
    // Core components
    std::unique_ptr<VulkanCore> core;
};

#endif // VULKANRENDERER_H
