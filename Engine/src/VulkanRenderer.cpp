#include "VulkanRenderer.h"
#include "core/VulkanCore.h"
#include "core/VulkanPipeline.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanSwapChain.h"
#include "core/VulkanImGui.h"
#include "rendering/TextureManager.h"
#include "Scene.h"
#include <stdexcept>

VulkanRenderer::VulkanRenderer() {
    core = std::make_unique<VulkanCore>();
}

VulkanRenderer::~VulkanRenderer() {
    core->cleanup();
}

void VulkanRenderer::initVulkan(GLFWwindow* window) {
    try {
        // Verificar drivers Vulkan
        uint32_t driverVersion;
        vkEnumerateInstanceVersion(&driverVersion);
        if (VK_VERSION_MAJOR(driverVersion) < 1) {
            throw std::runtime_error("Vulkan driver version too old. Please update your graphics drivers.");
        }
        
        // Verificar extensões necessárias
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        if (extensionCount == 0) {
            throw std::runtime_error("No Vulkan extensions found. Please check your graphics drivers.");
        }

        core = std::make_unique<VulkanCore>();
        core->init(window);
        textureManager = std::make_unique<TextureManager>(core.get());
        projectManager = std::make_unique<ProjectManager>(core.get());
        modelLoader = std::make_unique<EngineModelLoader>(*this);
    }
    catch (const std::exception& e) {
        std::string error = "VulkanRenderer initialization failed: ";
        error += e.what();
        throw std::runtime_error(error);
    }
}
