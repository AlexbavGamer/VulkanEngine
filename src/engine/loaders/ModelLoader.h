    #pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../../ecs/Component.h"
#include "Component.h"

class VulkanRenderer;

class EngineModelLoader {
public:
    EngineModelLoader(VulkanRenderer& renderer) : vulkanRenderer(renderer) {}
    bool LoadModel(const std::string& path, std::shared_ptr<Entity> entity);

private:
    void ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> entity);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Entity> entity);
    void ProcessMaterial(aiMesh* mesh, const aiScene* scene, MaterialComponent& materialComponent);
    
    void ConfigureTransform(std::shared_ptr<Entity> entity);
    std::vector<Vertex> ExtractVertices(aiMesh* mesh);
    std::vector<uint32_t> ExtractIndices(aiMesh* mesh);
    void CreateVertexBuffer(MeshComponent& meshComponent, const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(MeshComponent& meshComponent, const std::vector<uint32_t>& indices);
    
    void SetupDescriptors(MaterialComponent& material);
    void CreateMaterialPipeline(MaterialComponent& material);
    void AllocateDescriptorSet(MaterialComponent& material, VkDescriptorSetLayout layout);
    void CreateUniformBuffer(MaterialComponent& material);
    std::array<VkDescriptorImageInfo, 5> SetupImageInfos(MaterialComponent& material);
    void UpdateDescriptorSets(MaterialComponent& material, const std::array<VkDescriptorImageInfo, 5>& imageInfos);
    
    void CreateDefaultMaterial(MaterialComponent& material);
    void LoadMaterialTextures(aiMaterial* material, MaterialComponent& materialComponent);

    VulkanRenderer& vulkanRenderer;
    std::string directory;
};
