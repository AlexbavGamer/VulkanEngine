#include "TextureManager.h"
#include "Texture.h"
#include "../core/VulkanCore.h"
#include "../core/VulkanDescriptor.h"
#include "../core/VulkanPipeline.h" 
#include "../core/VulkanSwapChain.h"

#include <stb_image.h>
#include <iostream>

TextureManager::TextureManager(VulkanCore* core) : vulkanCore(core) {}

TextureManager::~TextureManager() {
    cleanup();
}

void TextureManager::cleanup() {
    textureCache.clear();
}

std::shared_ptr<Texture> TextureManager::loadTexture(const std::string& path) 
{
    std::cout << "\nLoading texture: " << path << std::endl;

    // Verifica se a textura já está carregada
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return it->second;
    }

    // Carrega a imagem usando stb_image
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    
    if (!pixels) {
        throw std::runtime_error("Failed to load texture image: " + path);
    }

    VkDeviceSize imageSize = width * height * 4;
    
    // Cria a textura
    auto texture = createTextureFromData(pixels, imageSize, width, height);
    
    // Libera os pixels
    stbi_image_free(pixels);
    
    // Cache a textura
    textureCache[path] = texture;
    
    return texture;
}

std::shared_ptr<Texture> TextureManager::createSolidColorTexture(const glm::vec4& color) {
    const uint32_t width = 1;
    const uint32_t height = 1;
    unsigned char data[4];
    
    // Converte cor float para RGBA 8-bit
    data[0] = static_cast<unsigned char>(color.r * 255.0f);
    data[1] = static_cast<unsigned char>(color.g * 255.0f);
    data[2] = static_cast<unsigned char>(color.b * 255.0f);
    data[3] = static_cast<unsigned char>(color.a * 255.0f);
    
    return createTextureFromData(data, 4, width, height);
}

std::shared_ptr<Texture> TextureManager::createDefaultNormalTexture() {
    const uint32_t width = 1;
    const uint32_t height = 1;
    unsigned char data[4] = {128, 128, 255, 255}; // Normal apontando para cima (0,0,1)
    
    return createTextureFromData(data, 4, width, height);
}

std::shared_ptr<Texture> TextureManager::createTextureFromData(
    const void* data, 
    VkDeviceSize size,
    uint32_t width,
    uint32_t height,
    VkFormat format
) {
    auto texture = std::make_shared<Texture>();
    
    // Criação da imagem e buffer temporário
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    vulkanCore->createBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );
    
    // Copia dados para o buffer
    vulkanCore->copyDataToBuffer(data, stagingBufferMemory, size);
    
    // Criação da imagem
    vulkanCore->createImage(
        width,
        height,
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        texture->image,
        texture->imageMemory
    );
    
    // Transição de layout e cópia do buffer para a imagem
    vulkanCore->transitionImageLayout(texture->image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vulkanCore->copyBufferToImage(stagingBuffer, texture->image, width, height);
    vulkanCore->transitionImageLayout(texture->image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    // Criação da imageView
    texture->imageView = vulkanCore->createImageView(texture->image, format, VK_IMAGE_ASPECT_COLOR_BIT);
    
    // Criação do sampler
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.mipLodBias = 0.0f;

    if (vkCreateSampler(vulkanCore->getDevice(), &samplerInfo, nullptr, &texture->sampler) != VK_SUCCESS) {
        throw std::runtime_error("Falha ao criar sampler!");
    }
    // Limpa recursos temporários
    vkDestroyBuffer(vulkanCore->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(vulkanCore->getDevice(), stagingBufferMemory, nullptr);
    
    return texture;
}