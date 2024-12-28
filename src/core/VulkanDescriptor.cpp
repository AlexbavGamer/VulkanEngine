#include "VulkanDescriptor.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"
#include "VulkanImGui.h"
#include "VulkanCore.h"

#include <stdexcept>

VulkanDescriptor::VulkanDescriptor(VulkanCore &core) : core(core), descriptorPool(VK_NULL_HANDLE), descriptorSetLayout(VK_NULL_HANDLE)
{
}

VulkanDescriptor::~VulkanDescriptor()
{
    cleanup();
}

void VulkanDescriptor::create()
{
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
}

void VulkanDescriptor::cleanup()
{
    auto device = core.getDevice();
    if (descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }

    for (auto pool : descriptorPools)
    {
        if (pool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
    }
    descriptorPools.clear();

    if (descriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        descriptorSetLayout = VK_NULL_HANDLE;
    }
}

void VulkanDescriptor::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};

    // Para UBOs
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(core.getMaxFramesInFlight() * 100);

    // Para Combined Image Samplers
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(core.getMaxFramesInFlight() * 500); // 5 texturas por material

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(core.getMaxFramesInFlight() * 100);

    if (vkCreateDescriptorPool(core.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanDescriptor::createDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
        {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // albedoMap
        {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // normalMap
        {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // metallicRoughnessMap
        {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // aoMap
        {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}  // emissiveMap
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{}; 
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(core.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("falha ao criar layout do descriptor set!");
    }
}

void VulkanDescriptor::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(core.getMaxFramesInFlight(), descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(core.getMaxFramesInFlight());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(core.getMaxFramesInFlight());

    if (vkAllocateDescriptorSets(core.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < core.getMaxFramesInFlight(); i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UBO);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(core.getDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

std::vector<VkDescriptorSetLayoutBinding> VulkanDescriptor::getDescriptorSetLayoutBindings() const
{
    return {
        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
        {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
        {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
        {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
        {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
        {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}};
}

void VulkanDescriptor::createUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = core.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

// Improved uniform buffer update with proper synchronization
void VulkanDescriptor::updateUniformBuffer(VkCommandBuffer commandBuffer, VkBuffer uniformBuffer, const UBO &ubo)
{
    VkDeviceSize bufferSize = sizeof(UBO);

    void *data;
    VkResult result = vkMapMemory(core.getDevice(), uniformBufferMemory, 0, bufferSize, 0, &data);
    if (result == VK_SUCCESS)
    {
        memcpy(data, &ubo, bufferSize);
        vkUnmapMemory(core.getDevice(), uniformBufferMemory);
    }
}

VkDescriptorBufferInfo VulkanDescriptor::getBufferInfo(VkBuffer buffer, VkDeviceSize size)
{

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = size;
    return bufferInfo;
}

VkDescriptorSet VulkanDescriptor::allocateDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(core.getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor set!");
    }

    return descriptorSet;
}

VkDescriptorSet VulkanDescriptor::createDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(core.getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor set!");
    }
    return descriptorSet;
}

void VulkanDescriptor::addDescriptorPool(VkDescriptorPool descriptorPool)
{
    descriptorPools.push_back(descriptorPool);
}