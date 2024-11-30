// Include necessary headers
#pragma once

#include <vulkan/vulkan.h>
#include "../components.h"
#include "../ecs/Registry.h"
#include "../ecs/Entity.h"

class RenderSystem {
public:
    RenderSystem() = default;
    ~RenderSystem() = default;
    
    void render(Registry& registry, VkCommandBuffer commandBuffer);
};