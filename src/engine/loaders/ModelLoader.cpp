#include "ModelLoader.h"
#include "../../VulkanRenderer.h"
#include "../../core/VulkanCore.h"
#include "../../core/VulkanDescriptor.h"
#include "../../rendering/TextureManager.h"

bool EngineModelLoader::LoadModel(const std::string& path, std::shared_ptr<Entity> entity) {
    Assimp::Importer importer;
    
    // Configurar flags para otimização
    unsigned int flags = aiProcess_Triangulate | 
                        aiProcess_GenNormals |
                        aiProcess_CalcTangentSpace |
                        aiProcess_JoinIdenticalVertices |
                        aiProcess_OptimizeMeshes;

    const aiScene* scene = importer.ReadFile(path, flags);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
        return false;
    }

    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene, entity);
    return true;
}

void EngineModelLoader::ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> entity) {
    // Processar todas as malhas do nó
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, entity);
    }

    // Processar nós filhos recursivamente
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, entity);
    }
}

void EngineModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Entity> entity) {
    auto& renderComponent = entity->AddOrGetComponent<RenderComponent>();
    auto& meshComponent = renderComponent.mesh;
    auto& materialComponent = renderComponent.material;

    std::cout << "\nProcessing mesh: " << mesh->mName.C_Str() << std::endl;
    std::cout << "Vertices: " << mesh->mNumVertices << std::endl;
    std::cout << "Has normals: " << (mesh->HasNormals() ? "yes" : "no") << std::endl;
    std::cout << "Has texture coords: " << (mesh->HasTextureCoords(0) ? "yes" : "no") << std::endl;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Processar vértices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        
        // Posição
        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        // Normal
        if(mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        // Coordenadas de textura
        if(mesh->mTextureCoords[0]) {
            vertex.texCoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        vertices.push_back(vertex);
    }

    // Processar índices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Criar buffers
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

    vulkanRenderer.getCore()->createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.vertexBuffer,
        meshComponent.vertexBufferMemory
    );

    vulkanRenderer.getCore()->createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.indexBuffer,
        meshComponent.indexBufferMemory
    );

    // Copiar dados para os buffers
    vulkanRenderer.getCore()->copyDataToBuffer(vertices.data(), meshComponent.vertexBufferMemory, vertexBufferSize);
    vulkanRenderer.getCore()->copyDataToBuffer(indices.data(), meshComponent.indexBufferMemory, indexBufferSize);
    meshComponent.indexCount = static_cast<uint32_t>(indices.size());

    // Processar material
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        LoadMaterialTextures(material, materialComponent);
    } else {
        CreateDefaultMaterial(materialComponent);
    }

    SetupDescriptors(materialComponent);
    
    // Configurar nome do componente de renderização
    renderComponent.name = mesh->mName.length > 0 ? mesh->mName.C_Str() : "Unnamed Mesh";
}

void EngineModelLoader::CreateDefaultMaterial(MaterialComponent &material) {
    material.albedoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    material.normalMap = vulkanRenderer.getTextureManager()->createDefaultNormalTexture();
    material.metallicRoughnessMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(0.0f, 0.5f, 0.0f, 0.0f));
    material.aoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    material.emissiveMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(0.0f));
}

void EngineModelLoader::SetupDescriptors(MaterialComponent &material) {
    // Layout bindings para UBO e texturas
    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // UBO
        {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // albedo
        {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // normal
        {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // metallic-roughness
        {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}, // ao
        {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}  // emissive
    };

    // Criar layout e descriptor set
    auto descriptorSetLayout = vulkanRenderer.getCore()->getDescriptor()->createDescriptorSetLayout(bindings);
    
    // Criar pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    if (vkCreatePipelineLayout(vulkanRenderer.getCore()->getDevice(), &pipelineLayoutInfo, nullptr, &material.pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // Criar uniform buffer para UBO
    VkDeviceSize bufferSize = sizeof(UBO);
    vulkanRenderer.getCore()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        material.uniformBuffer,
        material.uniformBufferMemory
    );

    // Criar descriptor set
    material.descriptorSet = vulkanRenderer.getCore()->getDescriptor()->createDescriptorSet(
        bindings,
        vulkanRenderer.getCore()->getDescriptor()->getBufferInfo(material.uniformBuffer, bufferSize),
        {
            material.albedoMap->getDescriptorInfo(),
            material.normalMap->getDescriptorInfo(),
            material.metallicRoughnessMap->getDescriptorInfo(),
            material.aoMap->getDescriptorInfo(),
            material.emissiveMap->getDescriptorInfo()
        }
    );

    // Criar pipeline
    material.pipeline = vulkanRenderer.getCore()->getPipeline()->createMaterialPipeline(
        material.pipelineLayout,
        "shaders/pbr.vert.spv",
        "shaders/pbr.frag.spv"
    );
}

void EngineModelLoader::LoadMaterialTextures(aiMaterial *material, MaterialComponent &materialComponent) {
    aiString texturePath;
    
    // Albedo/Base Color
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        materialComponent.albedoMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    } else {
        // Verifica propriedades de cor base
        aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
        materialComponent.albedoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(
            glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a)
        );
    }

    // Normal Map
    if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS) {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        materialComponent.normalMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    } else {
        materialComponent.normalMap = vulkanRenderer.getTextureManager()->createDefaultNormalTexture();
    }

    // Metallic-Roughness Map
    if (material->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS) {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        materialComponent.metallicRoughnessMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    } else {
        float metallicFactor = 0.0f;
        float roughnessFactor = 0.5f;
        material->Get(AI_MATKEY_METALLIC_FACTOR, metallicFactor);
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessFactor);
        materialComponent.metallicRoughnessMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(
            glm::vec4(0.0f, roughnessFactor, metallicFactor, 0.0f)
        );
    }

    // Ambient Occlusion
    if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS) {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        materialComponent.aoMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    } else {
        materialComponent.aoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    }

    // Emissive Map
    if (material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath) == AI_SUCCESS) {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        materialComponent.emissiveMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    } else {
        aiColor3D emissiveColor(0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
        materialComponent.emissiveMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(
            glm::vec4(emissiveColor.r, emissiveColor.g, emissiveColor.b, 1.0f)
        );
    }
}