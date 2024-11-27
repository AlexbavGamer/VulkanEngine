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
    Assimp::Importer importer;  // Add this line
    
    void ProcessNode(aiNode* node, const aiScene* scene, Entity& entity);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, Entity& entity);
    void LoadMaterialTextures(aiMaterial* mat, Entity& entity);
    public:
    explicit EngineModelLoader(VulkanRenderer& renderer) : vulkanRenderer(renderer) {}
    ~EngineModelLoader() = default;

    bool LoadModel(const std::string& path, Entity& entity);
};