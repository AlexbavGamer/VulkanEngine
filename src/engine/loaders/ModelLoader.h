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
    
    void ProcessNode(aiNode* node, const aiScene* scene, MeshComponent& meshComponent, MaterialComponent& materialComponent);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, MeshComponent& meshComponent, MaterialComponent& materialComponent);
    void LoadMaterialTextures(aiMaterial* mat, MaterialComponent& materialComponent);

public:
    explicit EngineModelLoader(VulkanRenderer& renderer) : vulkanRenderer(renderer) {}
    ~EngineModelLoader() = default;

    bool LoadModel(const std::string& path, Entity& entity);
};