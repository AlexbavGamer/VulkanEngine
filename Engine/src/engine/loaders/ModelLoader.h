#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <memory>
#include "../../core/VulkanCore.h"
#include "../../core/VulkanDescriptor.h"
#include "../../ecs/Component.h"
#include "../../ecs/Entity.h"
#include "../../ecs/components/MaterialComponent.h"
#include "../../ecs/components/MeshComponent.h"
#include "../../ecs/components/TransformComponent.h"
#include "../../rendering/TextureManager.h"

class VulkanRenderer;

class EngineModelLoader
{
public:
    EngineModelLoader(VulkanRenderer &renderer) : vulkanRenderer(renderer) {}
    std::shared_ptr<Entity> LoadModel(const std::string &path, std::shared_ptr<Entity> parentEntity = nullptr);

private:
    std::shared_ptr<Entity> ProcessNode(aiNode *node, const aiScene *scene, std::shared_ptr<Entity> parentEntity);
    void ProcessMesh(aiMesh *mesh, const aiScene *scene, std::shared_ptr<Entity> entity);
    void ProcessMaterial(aiMesh *mesh, const aiScene *scene, MaterialComponent &materialComponent);
    void ProcessTransform(aiNode *node, TransformComponent &transform);

    void ConfigureTransform(std::shared_ptr<Entity> entity);
    std::vector<Vertex> ExtractVertices(aiMesh *mesh);
    std::vector<uint32_t> ExtractIndices(aiMesh *mesh);
    void CreateVertexBuffer(MeshComponent &meshComponent, const std::vector<Vertex> &vertices);
    void CreateIndexBuffer(MeshComponent &meshComponent, const std::vector<uint32_t> &indices);

    void SetupDescriptors(MaterialComponent &material);
    void CreateMaterialPipeline(MaterialComponent &material);
    void AllocateDescriptorSet(MaterialComponent &material, VkDescriptorSetLayout layout);
    void CreateUniformBuffer(MaterialComponent &material);
    void CreateLightUniformBuffer(MaterialComponent &material);
    std::array<VkDescriptorImageInfo, 5> SetupImageInfos(MaterialComponent &material);
    void UpdateDescriptorSets(MaterialComponent &material, const std::array<VkDescriptorImageInfo, 5> &imageInfos);
    void CreateDefaultMaterial(MaterialComponent &material);
    void LoadMaterialTextures(aiMaterial *material, MaterialComponent &materialComponent);

    VulkanRenderer &vulkanRenderer;
    std::string directory;
};
