#include "ShaderManager.h"
#include <fstream>
#include <vector>

void ShaderManager::watchShader(const std::string& path) {
    watcher.addPath(path);
}

void ShaderManager::reloadIfNeeded() {
    auto changedFiles = watcher.getChangedFiles();
    
    for (const auto& path : changedFiles) {
        if (shaders.find(path) != shaders.end()) {
            // Destroy old shader module
            vkDestroyShaderModule(device, shaders[path], nullptr);
            
            // Create new shader module
            createShaderModule(path);
        }
    }
}

VkShaderModule ShaderManager::createShaderModule(const std::string& path) {
    std::vector<char> code = readFile(path);
    
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule shaderModule;
    vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    
    shaders[path] = shaderModule;
    return shaderModule;
}

std::vector<char> ShaderManager::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    size_t fileSize = (size_t)file.tellg();
    
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    
    return buffer;
}

void ShaderManager::cleanup() {
    for (auto& shader : shaders) {
        vkDestroyShaderModule(device, shader.second, nullptr);
    }
    shaders.clear();
}
