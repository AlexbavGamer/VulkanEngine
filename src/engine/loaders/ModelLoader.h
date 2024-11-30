#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include "../../Components.h"
#include "../../VulkanRenderer.h"

class EngineModelLoader {
private:
    VulkanRenderer& vulkanRenderer;
    std::string directory;
    Assimp::Importer importer;
    
    void ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> entity, MaterialComponent& materialComponent, MeshComponent& meshCompoonent);;
    void ProcessMesh(aiMesh* mesh, aiNode* node, const aiScene* scene, std::shared_ptr<Entity> entity, MaterialComponent& materialComponent, MeshComponent& meshComponent);
    void LoadMaterialTextures(aiMaterial* mat, std::shared_ptr<Entity> entity, MaterialComponent& materialComponent);
    void SetDefaultMaterial(MaterialComponent &materialComponent);

public:
    explicit EngineModelLoader(VulkanRenderer& renderer) : vulkanRenderer(renderer) {}
    ~EngineModelLoader() = default;

    bool LoadModel(const std::string& path, std::shared_ptr<Entity> entity);
};
