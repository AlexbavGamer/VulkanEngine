#include "ModelLoader.h"
#include <filesystem>
#include <magic_enum.hpp>

bool EngineModelLoader::LoadModel(const std::string& path, std::shared_ptr<Entity> entity) {
    uint32_t flags = aiProcess_Triangulate | 
                     aiProcess_GenNormals | 
                     aiProcess_CalcTangentSpace |
                     aiProcess_JoinIdenticalVertices |
                     aiProcess_OptimizeMeshes |
                     aiProcess_FlipUVs;

    const aiScene* scene = importer.ReadFile(path, flags);
    
    if (!scene || !scene->mRootNode) {
        std::cerr << "Model loading failed: " << path << " - " << importer.GetErrorString() << std::endl;
        return false;
    }

    MaterialComponent& materialComponent = entity->addComponent<MaterialComponent>();
    MeshComponent& meshComponent = entity->addComponent<MeshComponent>();

    directory = std::filesystem::path(path).parent_path().string();
    std::replace(directory.begin(), directory.end(), '\\', '/');

    ProcessNode(scene->mRootNode, scene, entity, materialComponent, meshComponent);
    return true;
}

void EngineModelLoader::ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> entity, MaterialComponent& materialComponent, MeshComponent& meshComponent) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, node, scene, entity, materialComponent, meshComponent);
    }
    
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, entity, materialComponent, meshComponent);
    }
}

void EngineModelLoader::ProcessMesh(aiMesh* mesh, aiNode* node, const aiScene* scene, std::shared_ptr<Entity> entity, MaterialComponent& materialComponent, MeshComponent& meshComponent) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Process vertices
    vertices.reserve(mesh->mNumVertices);
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        
        if(mesh->HasNormals()) {
            vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }
        
        if(mesh->mTextureCoords[0]) {
            vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.texCoord = {0.0f, 0.0f};
        }
        vertices.push_back(vertex);
    }

    // Process indices
    indices.reserve(mesh->mNumFaces * 3);
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Create components
    RenderComponent& renderComponent = entity->AddOrGetComponent<RenderComponent>();

    // Create buffers
    vulkanRenderer.getCore()->createBuffer(
        vertices.size() * sizeof(Vertex),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.vertexBuffer,
        meshComponent.vertexBufferMemory
    );

    vulkanRenderer.getCore()->createBuffer(
        indices.size() * sizeof(uint32_t),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.indexBuffer,
        meshComponent.indexBufferMemory
    );

    // Copy data
    vulkanRenderer.getCore()->copyDataToBuffer(vertices.data(), meshComponent.vertexBufferMemory, vertices.size() * sizeof(Vertex));
    vulkanRenderer.getCore()->copyDataToBuffer(indices.data(), meshComponent.indexBufferMemory, indices.size() * sizeof(uint32_t));
    meshComponent.indexCount = static_cast<uint32_t>(indices.size());

    // Process material
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        LoadMaterialTextures(material, entity, materialComponent);
    } else {
        SetDefaultMaterial(materialComponent);
    }

    // Create uniform buffer
    VkDeviceSize bufferSize = sizeof(UBO);
    vulkanRenderer.getCore()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        materialComponent.uniformBuffer,
        materialComponent.uniformBufferMemory
    );

    // Set up descriptor set
    materialComponent.descriptorSet = vulkanRenderer.getCore()->getDescriptor()->allocateDescriptorSet();

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = materialComponent.uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = bufferSize;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = materialComponent.hasTexture ? materialComponent.textureImageView : vulkanRenderer.getCore()->getDefaultTextureView();
    imageInfo.sampler = vulkanRenderer.getCore()->getTextureSampler();

    vulkanRenderer.getCore()->getDescriptor()->updateDescriptorSet(
        materialComponent.descriptorSet,
        bufferInfo,
        imageInfo
    );

    renderComponent.material = materialComponent;
    renderComponent.mesh = meshComponent;
    renderComponent.name = node->mName.C_Str();
}

void EngineModelLoader::LoadMaterialTextures(aiMaterial* mat, std::shared_ptr<Entity> entity, MaterialComponent& materialComponent) {
    
    // Create uniform buffer
    VkDeviceSize bufferSize = sizeof(UBO);
    vulkanRenderer.getCore()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        materialComponent.uniformBuffer,
        materialComponent.uniformBufferMemory
    );

    // Load color if texture is not available
    aiColor4D color;
    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        materialComponent.color = glm::vec4(color.r, color.g, color.b, color.a);
    } else {
        // Set a default color if no color is found
        materialComponent.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White color
    }

    // Load texture if available
    aiString texturePath;
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
        
        if (std::filesystem::exists(fullPath)) {
            vulkanRenderer.getCore()->createTextureImage(
                fullPath.c_str(),
                materialComponent.textureImage,
                materialComponent.textureImageMemory
            );
            
            materialComponent.textureImageView = vulkanRenderer.getCore()->createImageView(
                materialComponent.textureImage,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_ASPECT_COLOR_BIT
            );
            materialComponent.hasTexture = true;
        }
    } else {
        // If no texture is found, ensure hasTexture is false
        materialComponent.hasTexture = false;
    }
    
    materialComponent.descriptorSet = vulkanRenderer.getCore()->getDescriptor()->allocateDescriptorSet();

    // Update descriptor set with both uniform buffer and texture
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = materialComponent.uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = bufferSize;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = materialComponent.hasTexture ? 
        materialComponent.textureImageView : 
        vulkanRenderer.getCore()->getDefaultTextureView(); // Use a default texture view if no texture
    imageInfo.sampler = vulkanRenderer.getCore()->getTextureSampler();

    vulkanRenderer.getCore()->getDescriptor()->updateDescriptorSet(
        materialComponent.descriptorSet,
        bufferInfo,
        imageInfo
    );

    materialComponent.pipeline = vulkanRenderer.getCore()->getPipeline()->getPipeline();
    materialComponent.pipelineLayout = vulkanRenderer.getCore()->getPipeline()->getLayout();
}

void EngineModelLoader::SetDefaultMaterial(MaterialComponent& materialComponent) {
    // Set default material properties
    materialComponent.color = glm::vec4(1.0f);
    materialComponent.metallic = 0.0f;
    materialComponent.roughness = 0.5f;
    materialComponent.ambientOcclusion = 1.0f;
    materialComponent.hasTexture = false;

    // Use default texture view
    materialComponent.textureImageView = vulkanRenderer.getCore()->getDefaultTextureView();

    std::cout << "Using default material" << std::endl;
}
