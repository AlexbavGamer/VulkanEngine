#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <string>
#include <string>
#include <stdexcept>

class TextureManager {
public:
    struct Texture {
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
        VkSampler sampler;
    };
    
    std::unordered_map<std::string, Texture> textures;

public:
    // Core texture loading functions
    Texture* loadTexture(const std::string& path);
    void createTextureImage(const std::string& path, TextureManager::Texture& texture);
    void createTextureImageView(TextureManager::Texture& texture);
    void createTextureSampler(TextureManager::Texture& texture);

    // Memory management functions
    void cleanup();
    void destroyTexture(Texture& texture);

    // Utility functions
    bool hasTexture(const std::string& path) const;
    void removeTexture(const std::string& path);
    
    // Constructor/Destructor
    TextureManager() = default;
    ~TextureManager();

private:
    // Vulkan device reference (you'll need to add this)
    VkDevice device;
    
    // Helper functions
    void transitionImageLayout(VkImage image, VkFormat format, 
                             VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, 
                          uint32_t width, uint32_t height);
};
