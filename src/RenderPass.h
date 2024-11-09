#pragma once

#include <vulkan/vulkan.h>


class RenderPassBuilder {
public:
    RenderPassBuilder& addColorAttachment();
    RenderPassBuilder& addDepthAttachment();
    VkRenderPass build();
};
