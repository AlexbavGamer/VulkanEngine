#pragma once
#include <vulkan/vulkan.h>

class Texture
{
public:
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;

    void Destroy(VkDevice device)
    {
        // Destruir o sampler (se existir)
        if (sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(device, sampler, nullptr);
            sampler = VK_NULL_HANDLE;
        }

        // Destruir a image view (se existir)
        if (imageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(device, imageView, nullptr);
            imageView = VK_NULL_HANDLE;
        }

        // Destruir a imagem (se existir)
        if (image != VK_NULL_HANDLE)
        {
            vkDestroyImage(device, image, nullptr);
            image = VK_NULL_HANDLE;
        }

        // Liberar a memória da imagem (se existir)
        if (imageMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, imageMemory, nullptr);
            imageMemory = VK_NULL_HANDLE;
        }
    }

    VkDescriptorImageInfo getDescriptorInfo() const
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = imageView;
        imageInfo.sampler = sampler;
        return imageInfo;
    }
};