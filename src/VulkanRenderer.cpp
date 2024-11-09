#include "VulkanRenderer.h"
#include <stdexcept>

VulkanRenderer::VulkanRenderer() {
    core = std::make_unique<VulkanCore>();
}

VulkanRenderer::~VulkanRenderer() {
    core->cleanup();
}

void VulkanRenderer::initVulkan(GLFWwindow* window) {
    // Initialize core components
    core->init(window);
}

