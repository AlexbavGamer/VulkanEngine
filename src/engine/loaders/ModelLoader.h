
#pragma once

#include <string>

#include <memory>

#include <assimp/Importer.hpp>

#include <assimp/scene.h>

#include <assimp/postprocess.h>

#include "../../ecs/Entity.h"
#include "../../Components.h"

class VulkanRenderer;

class EngineModelLoader {

public:
    EngineModelLoader(VulkanRenderer& renderer) : vulkanRenderer(renderer) {}
    bool LoadModel(const std::string& path, std::shared_ptr<Entity> entity);
private:

    void ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> entity);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Entity> entity);
    void CreateDefaultMaterial(MaterialComponent& material);
    void SetupDescriptors(MaterialComponent& material);
    void LoadMaterialTextures(aiMaterial* material, MaterialComponent& materialComponent);

    std::string directory;
    VulkanRenderer& vulkanRenderer;
};
