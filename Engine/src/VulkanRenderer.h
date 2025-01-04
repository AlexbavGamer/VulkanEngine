#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "Singleton.h"
#include "Scene.h"
#include "core/VulkanCore.h"

#include "core/VulkanPipeline.h"
#include "core/VulkanDescriptor.h"
#include "core/VulkanSwapChain.h"
#include "core/VulkanImGui.h"
#include "rendering/TextureManager.h"
#include "project/projectManagment.h"
#include "engine/loaders/ModelLoader.h"

class VulkanRenderer : public Singleton<VulkanRenderer> {
    friend class Singleton<VulkanRenderer>;

public:
    VulkanRenderer();
    ~VulkanRenderer();

    void initVulkan(GLFWwindow* window);
    VulkanCore* getCore() { return core.get(); }
    TextureManager* getTextureManager() { return textureManager.get(); }
    ProjectManager* getProjectManager() { return projectManager.get(); }
    EngineModelLoader* getModelLoader() { return modelLoader.get(); }

private:
    std::unique_ptr<VulkanCore> core;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<ProjectManager> projectManager;
    std::unique_ptr<EngineModelLoader> modelLoader;
};