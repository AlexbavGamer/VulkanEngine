// Include necessary headers
#pragma once

#include <vulkan/vulkan.h>
#include "../ecs/Registry.h"
#include "../ecs/Entity.h"

struct UBO;
struct TransformComponent;
class VulkanRenderer;

class RenderSystem {
public:
    RenderSystem() = default;
    ~RenderSystem() = default;
    
    void render(Registry& registry, VkCommandBuffer commandBuffer);
    UBO prepareUBO(const TransformComponent &transform, VulkanRenderer &vulkanRender);
};