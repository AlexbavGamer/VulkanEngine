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
    core->init(window);
    textureManager = std::make_unique<TextureManager>(core.get());
    projectManager = std::make_unique<ProjectManager>(core.get());
    modelLoader = std::make_unique<EngineModelLoader>(*this);
}

