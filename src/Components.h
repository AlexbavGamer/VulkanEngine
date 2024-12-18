#pragma once

#include "core/VulkanTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>
#include "ecs/Component.h"

#include "imgui.h"

class VulkanRenderer;

struct TransformComponent : Component {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 getMatrix() const {
        return glm::translate(glm::mat4(1.0f), position) *
               glm::yawPitchRoll(rotation.x, rotation.y, rotation.z) *
               glm::scale(glm::mat4(1.0f), scale);
    }

    void renderComponent() override 
    {
        ImGui::Text("Transform Component");

        if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::BeginChild("PositionChild", ImVec2(0, 100), true)) {
                ImGui::DragFloat("Position X", &position.x, 0.1f);
                ImGui::DragFloat("Position Y", &position.y, 0.1f);
                ImGui::DragFloat("Position Z", &position.z, 0.1f);
            }
            ImGui::EndChild();
        }

        if (ImGui::CollapsingHeader("Rotation", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::BeginChild("RotationChild", ImVec2(0, 100), true)) {
                ImGui::DragFloat("Rotation X", &rotation.x, 0.1f);
                ImGui::DragFloat("Rotation Y", &rotation.y, 0.1f);
                ImGui::DragFloat("Rotation Z", &rotation.z, 0.1f);
            }
            ImGui::EndChild();
        }

        if (ImGui::CollapsingHeader("Scale", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::BeginChild("ScaleChild", ImVec2(0, 100), true)) {
                ImGui::DragFloat("Scale X", &scale.x, 0.1f);
                ImGui::DragFloat("Scale Y", &scale.y, 0.1f);
                ImGui::DragFloat("Scale Z", &scale.z, 0.1f);
            }
            ImGui::EndChild();
        }
    }
};

struct MeshComponent : Component {
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
};

struct MaterialComponent : public Component {
    MaterialComponent()
    {
        pipeline = VK_NULL_HANDLE;
        pipelineLayout = VK_NULL_HANDLE;
        descriptorSet = VK_NULL_HANDLE;
        uniformBuffer = VK_NULL_HANDLE;
        uniformBufferMemory = VK_NULL_HANDLE;
        textureImage = VK_NULL_HANDLE;
        textureImageMemory = VK_NULL_HANDLE;
        textureImageView = VK_NULL_HANDLE;
        textureSampler = VK_NULL_HANDLE;
        descriptorSetLayout = VK_NULL_HANDLE;
        hasTexture = false;
    }
    
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSet descriptorSet;
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    // Material properties
    glm::vec4 color = glm::vec4(1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ambientOcclusion = 1.0f;
    
    // Texture properties
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    bool hasTexture = false;

    // Sampler and descriptor set
    VkSampler textureSampler;
    VkDescriptorSetLayout descriptorSetLayout;
};


struct LightComponent : Component {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    glm::vec3 direction;
};

// Definição do CameraComponent que estava ausente no seu arquivo
struct CameraComponent {
    glm::mat4 projection;
    glm::mat4 view;

    float width;
    float height;

    float yaw = -90.0f;   // Initial yaw angle
    float pitch = 0.0f;   // Initial pitch angle
    float sensitivity = 0.1f;  // Mouse sensitivity
    float fov = 90.0f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    void handleMouseMovement(double deltaX, double deltaY) {
        yaw += deltaX * sensitivity;
        pitch += deltaY * sensitivity;
        
        // Constrain pitch to avoid camera flipping
        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;
            
        // Calculate new front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
        
        // Update view matrix
        view = glm::lookAt(position, position + front, up);
    }

    glm::mat4 getViewProjection() const {
        return projection * view;
    }
};

struct UBO {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) Material material;
};


struct RenderComponent : Component
{
    std::string name;
    MeshComponent mesh;
    MaterialComponent material;

    void renderComponent() override 
    {
        ImGui::Text("Render Component");
        ImGui::InputText("Name", &name[0], name.size() + 1);
    }
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

    bool operator==(const Vertex &other) const {
      return position == other.position &&
            normal == other.normal &&
            texCoord == other.texCoord;
    }

    static std::vector<VkVertexInputBindingDescription> getBindingDescription() 
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)});

        return attributeDescriptions;
    }
};

namespace camera {
    void updateCamera(CameraComponent& camera, const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
}