#include "ModelLoader.h"
#include "../../VulkanRenderer.h"
#include "../../core/VulkanCore.h"
#include "../../core/VulkanDescriptor.h"
#include "../../rendering/TextureManager.h"

bool EngineModelLoader::LoadModel(const std::string &path, std::shared_ptr<Entity> entity)
{
    Assimp::Importer importer;

    // Configurar flags para otimização
    unsigned int flags = aiProcess_Triangulate |
                         aiProcess_GenNormals |
                         aiProcess_CalcTangentSpace |
                         aiProcess_JoinIdenticalVertices |
                         aiProcess_OptimizeMeshes;

    const aiScene *scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
        return false;
    }

    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene, entity);
    return true;
}

void EngineModelLoader::ProcessNode(aiNode *node, const aiScene *scene, std::shared_ptr<Entity> entity)
{
    // Processar todas as malhas do nó
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, entity);
    }

    // Processar nós filhos recursivamente
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, entity);
    }
}

void EngineModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene, std::shared_ptr<Entity> entity)
{
    auto &transform = entity->AddOrGetComponent<TransformComponent>();
    transform.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    transform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    transform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

    auto &renderComponent = entity->AddOrGetComponent<RenderComponent>();
    auto &meshComponent = renderComponent.mesh;
    auto &materialComponent = renderComponent.material;

    std::cout << "\nProcessing mesh: " << mesh->mName.C_Str() << std::endl;
    std::cout << "Vertices: " << mesh->mNumVertices << std::endl;
    std::cout << "Faces: " << mesh->mNumFaces << std::endl;
    std::cout << "Has normals: " << (mesh->HasNormals() ? "yes" : "no") << std::endl;
    std::cout << "Has texture coords: " << (mesh->HasTextureCoords(0) ? "yes" : "no") << std::endl;
    std::cout << "Material ID: " << mesh->mMaterialIndex << std::endl;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Processar vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};

        // Posição
        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z};

        // Normal
        if (mesh->HasNormals())
        {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z};
        }

        // Coordenadas de textura
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y};
        }

        vertices.push_back(vertex);
    }

    // Processar índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
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
        meshComponent.vertexBufferMemory);

    vulkanRenderer.getCore()->createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.indexBuffer,
        meshComponent.indexBufferMemory);

    // Copiar dados para os buffers
    vulkanRenderer.getCore()->copyDataToBuffer(vertices.data(), meshComponent.vertexBufferMemory, vertexBufferSize);
    vulkanRenderer.getCore()->copyDataToBuffer(indices.data(), meshComponent.indexBufferMemory, indexBufferSize);
    meshComponent.indexCount = static_cast<uint32_t>(indices.size());

    // Processar material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        LoadMaterialTextures(material, materialComponent);
    }
    else
    {
        CreateDefaultMaterial(materialComponent);
    }

    SetupDescriptors(materialComponent);

    // Configurar nome do componente de renderização
    renderComponent.name = mesh->mName.length > 0 ? mesh->mName.C_Str() : "Unnamed Mesh";
}

void EngineModelLoader::CreateDefaultMaterial(MaterialComponent &material)
{
    material.albedoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    material.normalMap = vulkanRenderer.getTextureManager()->createDefaultNormalTexture();
    material.metallicRoughnessMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(0.0f, 0.5f, 0.0f, 0.0f));
    material.aoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    material.emissiveMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(0.0f));
}

void EngineModelLoader::SetupDescriptors(MaterialComponent &material) {
    auto sceneDescriptorSetLayout = vulkanRenderer.getCore()->getSceneDescriptorSetLayout();
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &sceneDescriptorSetLayout;

    if (vkCreatePipelineLayout(vulkanRenderer.getCore()->getDevice(), &pipelineLayoutInfo, nullptr, &material.pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    material.descriptorSet = vulkanRenderer.getCore()->getSceneDescriptorSet();

    material.pipeline = vulkanRenderer.getCore()->getPipeline()->createMaterialPipeline(
        material.pipelineLayout,
        "shaders/pbr.vert.spv",
        "shaders/pbr.frag.spv"
    );
}

void EngineModelLoader::LoadMaterialTextures(aiMaterial *material, MaterialComponent &materialComponent)
{
    aiString texturePath;
    std::cout << "Starting material texture loading..." << std::endl;

    // Albedo/Base Color
    std::cout << "Loading Albedo/Base Color texture..." << std::endl;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
    {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        std::cout << "Found albedo texture at: " << fullPath << std::endl;
        materialComponent.albedoMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    }
    else
    {
        aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
        std::cout << "No albedo texture found. Creating solid color texture with values: ("
                  << baseColor.r << ", " << baseColor.g << ", " << baseColor.b << ", " << baseColor.a << ")" << std::endl;
        materialComponent.albedoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(
            glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a));
    }

    // Normal Map
    std::cout << "Loading Normal Map texture..." << std::endl;
    if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
    {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        std::cout << "Found normal map at: " << fullPath << std::endl;
        materialComponent.normalMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    }
    else
    {
        std::cout << "No normal map found. Creating default normal texture." << std::endl;
        materialComponent.normalMap = vulkanRenderer.getTextureManager()->createDefaultNormalTexture();
    }

    // Metallic-Roughness Map
    std::cout << "Loading Metallic-Roughness Map texture..." << std::endl;
    if (material->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS)
    {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        std::cout << "Found metallic-roughness map at: " << fullPath << std::endl;
        materialComponent.metallicRoughnessMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    }
    else
    {
        float metallicFactor = 0.0f;
        float roughnessFactor = 0.5f;
        material->Get(AI_MATKEY_METALLIC_FACTOR, metallicFactor);
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessFactor);
        std::cout << "No metallic-roughness map found. Creating solid color texture with metallic: "
                  << metallicFactor << ", roughness: " << roughnessFactor << std::endl;
        materialComponent.metallicRoughnessMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(
            glm::vec4(0.0f, roughnessFactor, metallicFactor, 0.0f));
    }

    // Ambient Occlusion
    std::cout << "Loading Ambient Occlusion texture..." << std::endl;
    if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS)
    {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        std::cout << "Found ambient occlusion map at: " << fullPath << std::endl;
        materialComponent.aoMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    }
    else
    {
        std::cout << "No ambient occlusion map found. Creating solid white texture." << std::endl;
        materialComponent.aoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    }

    // Emissive Map
    std::cout << "Loading Emissive Map texture..." << std::endl;
    if (material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath) == AI_SUCCESS)
    {
        std::string fullPath = directory + "/" + texturePath.C_Str();
        std::cout << "Found emissive map at: " << fullPath << std::endl;
        materialComponent.emissiveMap = vulkanRenderer.getTextureManager()->loadTexture(fullPath);
    }
    else
    {
        aiColor3D emissiveColor(0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
        std::cout << "No emissive map found. Creating solid color texture with values: ("
                  << emissiveColor.r << ", " << emissiveColor.g << ", " << emissiveColor.b << ")" << std::endl;
        materialComponent.emissiveMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(
            glm::vec4(emissiveColor.r, emissiveColor.g, emissiveColor.b, 1.0f));
    }

    std::cout << "Material texture loading completed." << std::endl;
}