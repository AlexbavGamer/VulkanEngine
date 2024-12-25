#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

// Forward declarations
class VulkanCore;
struct Texture;

class TextureManager {
public:
    explicit TextureManager(VulkanCore* core);
    ~TextureManager();

    std::shared_ptr<Texture> loadTexture(const std::string& path);
    std::shared_ptr<Texture> createSolidColorTexture(const glm::vec4& color);
    std::shared_ptr<Texture> createDefaultNormalTexture();
    void cleanup();

private:
    std::shared_ptr<Texture> createTextureFromData(
        const void* data, 
        VkDeviceSize size,
        uint32_t width,
        uint32_t height,
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB
    );

    VulkanCore* vulkanCore;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
};