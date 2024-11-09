#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>
#include "FileWatcher.h"

class ShaderManager {
private:
    std::unordered_map<std::string, VkShaderModule> shaders;
    FileWatcher watcher;
    VkDevice device;  // Added device member

public:
    // Added constructor to initialize device
    ShaderManager(VkDevice device) : device(device) {}
    
    void watchShader(const std::string& path);
    void reloadIfNeeded();
    VkShaderModule createShaderModule(const std::string& path);
    std::vector<char> readFile(const std::string& path);
    void cleanup();
};
