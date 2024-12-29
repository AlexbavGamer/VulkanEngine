#pragma once
#include "../Component.h"
#include "../../rendering/Texture.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <glm/glm.hpp>

struct MaterialComponent : public Component
{
    std::shared_ptr<Texture> albedoMap;
    std::shared_ptr<Texture> normalMap;
    std::shared_ptr<Texture> metallicRoughnessMap;
    std::shared_ptr<Texture> aoMap;
    std::shared_ptr<Texture> emissiveMap;

    VkDescriptorSet descriptorSet;
    VkBuffer uniformBuffer;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    VkDeviceMemory uniformBufferMemory;

    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    glm::vec3 emissiveFactor = glm::vec3(0.0f);
};