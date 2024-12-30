#include "ModelLoader.h"
#include "../../VulkanRenderer.h"

bool EngineModelLoader::LoadModel(const std::string &path, std::shared_ptr<Entity> entity)
{
    Assimp::Importer importer;
    unsigned int flags = aiProcess_Triangulate |
                         aiProcess_GenNormals |
                         aiProcess_CalcTangentSpace |
                         aiProcess_JoinIdenticalVertices |
                         aiProcess_OptimizeMeshes;

    const aiScene *scene = importer.ReadFile(path, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error("Falha ao carregar modelo: " + std::string(importer.GetErrorString()));
    }

    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene, entity);
    return true;
}

void EngineModelLoader::ConfigureTransform(std::shared_ptr<Entity> entity)
{
    auto &transform = entity->AddOrGetComponent<TransformComponent>();
    transform.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    transform.setRotation(glm::vec3(0.0f));
    transform.setScale(glm::vec3(1.0f));
}

std::vector<Vertex> EngineModelLoader::ExtractVertices(aiMesh *mesh)
{
    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};
        vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        if (mesh->HasNormals())
        {
            vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }

        vertices.push_back(vertex);
    }

    return vertices;
}

std::vector<uint32_t> EngineModelLoader::ExtractIndices(aiMesh *mesh)
{
    std::vector<uint32_t> indices;
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return indices;
}

void EngineModelLoader::CreateVertexBuffer(MeshComponent &meshComponent, const std::vector<Vertex> &vertices)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vulkanRenderer.getCore()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.vertexBuffer,
        meshComponent.vertexBufferMemory);

    void *data;
    vkMapMemory(vulkanRenderer.getCore()->getDevice(), meshComponent.vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), bufferSize);
    vkUnmapMemory(vulkanRenderer.getCore()->getDevice(), meshComponent.vertexBufferMemory);
}

void EngineModelLoader::CreateIndexBuffer(MeshComponent &meshComponent, const std::vector<uint32_t> &indices)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vulkanRenderer.getCore()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.indexBuffer,
        meshComponent.indexBufferMemory);

    void *data;
    vkMapMemory(vulkanRenderer.getCore()->getDevice(), meshComponent.indexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), bufferSize);
    vkUnmapMemory(vulkanRenderer.getCore()->getDevice(), meshComponent.indexBufferMemory);

    meshComponent.indexCount = static_cast<uint32_t>(indices.size());
}

void EngineModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene, std::shared_ptr<Entity> entity)
{
    ConfigureTransform(entity);

    auto &renderComponent = entity->AddOrGetComponent<RenderComponent>();
    auto &meshComponent = renderComponent.mesh;
    auto &materialComponent = renderComponent.material;

    auto vertices = ExtractVertices(mesh);
    auto indices = ExtractIndices(mesh);

    CreateVertexBuffer(meshComponent, vertices);
    CreateIndexBuffer(meshComponent, indices);

    ProcessMaterial(mesh, scene, materialComponent);
    CreateMaterialPipeline(materialComponent);
    SetupDescriptors(materialComponent);

    renderComponent.name = mesh->mName.length > 0 ? mesh->mName.C_Str() : "Unnamed Mesh";
}

void EngineModelLoader::CreateUniformBuffer(MaterialComponent &material)
{
    VkDeviceSize bufferSize = sizeof(UBO);
    vulkanRenderer.getCore()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        material.uniformBuffer,
        material.uniformBufferMemory);
}

void EngineModelLoader::UpdateDescriptorSets(MaterialComponent &material, const std::array<VkDescriptorImageInfo, 5> &imageInfos)
{
    std:: array<VkWriteDescriptorSet, 6> descriptorWrites{};

    VkDescriptorBufferInfo bufferInfo{material.uniformBuffer, 0, sizeof(UBO)};

    descriptorWrites[0] = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = material.descriptorSet,
        .dstBinding = 0, // Uniform buffer binding
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo};

    for (size_t i = 0; i < 5; i++)
    {
        descriptorWrites[i + 1] = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = material.descriptorSet,
            .dstBinding = static_cast<uint32_t>(i + 1), // Combined image sampler bindings
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfos[i]}; // Ensure imageInfos[i] contains valid VkDescriptorImageInfo
    }

    // Ensure the sampler is valid before updating the descriptor set
    std::cout << "Albedo map sampler: " << material.albedoMap->sampler << std::endl; // Debug log for albedo map sampler
    if (material.albedoMap && material.albedoMap->sampler != VK_NULL_HANDLE) {
        vkUpdateDescriptorSets(vulkanRenderer.getCore()->getDevice(),
            static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
    else {
        throw std::runtime_error("Invalid sampler for albedo map!");
    }
}

void EngineModelLoader::CreateDefaultMaterial(MaterialComponent &material)
{
    material.albedoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    material.normalMap = vulkanRenderer.getTextureManager()->createDefaultNormalTexture();
    material.metallicRoughnessMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(0.0f, 0.5f, 0.0f, 0.0f));
    material.aoMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(1.0f));
    material.emissiveMap = vulkanRenderer.getTextureManager()->createSolidColorTexture(glm::vec4(0.0f));
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

void EngineModelLoader::ProcessNode(aiNode *node, const aiScene *scene, std::shared_ptr<Entity> entity)
{
    // Processa todos os meshes do nó atual
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, entity);
    }

    // Processa recursivamente os nós filhos
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, entity);
    }
}

void EngineModelLoader::ProcessMaterial(aiMesh *mesh, const aiScene *scene, MaterialComponent &materialComponent)
{
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        LoadMaterialTextures(material, materialComponent);
    }
    else
    {
        CreateDefaultMaterial(materialComponent);
    }
}

void EngineModelLoader::SetupDescriptors(MaterialComponent &material)
{
    CreateUniformBuffer(material);
    AllocateDescriptorSet(material, material.descriptorSetLayout);
    auto imageInfos = SetupImageInfos(material);
    UpdateDescriptorSets(material, imageInfos);
}

void EngineModelLoader::CreateMaterialPipeline(MaterialComponent &material)
{
    // Criação do descriptor set layout
    std::vector<VkDescriptorSetLayoutBinding> bindings(6);

    // Binding para o UBO
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // Bindings para as 5 texturas
    for (size_t i = 1; i <= 5; ++i) {
        bindings[i].binding = i; // 1, 2, 3, 4, 5
        bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // Para texturas
        bindings[i].descriptorCount = 1;
        bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // Apenas no fragment shader
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(vulkanRenderer.getCore()->getDevice(), &layoutInfo, nullptr, &material.descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Falha ao criar descriptor set layout!");
    }

    // Criação do pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &material.descriptorSetLayout;

    if (vkCreatePipelineLayout(vulkanRenderer.getCore()->getDevice(), &pipelineLayoutInfo, nullptr, &material.pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Falha ao criar pipeline layout!");
    }

    // Agora você pode criar o pipeline usando material.pipelineLayout
    material.pipeline = vulkanRenderer.getCore()->getPipeline()->createMaterialPipeline(
        material.pipelineLayout,
        material.descriptorSetLayout,
        "shaders/pbr.vert.spv", // Caminho do shader de vértice
        "shaders/pbr.frag.spv"  // Caminho do shader de fragmento
    );
}

void EngineModelLoader::AllocateDescriptorSet(MaterialComponent &material, VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanRenderer.getCore()->getDescriptor()->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout; // Use o layout correto

    if (vkAllocateDescriptorSets(vulkanRenderer.getCore()->getDevice(), &allocInfo, &material.descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Falha ao alocar descriptor set!");
    }
}

std::array<VkDescriptorImageInfo, 5> EngineModelLoader::SetupImageInfos(MaterialComponent &material)
{
    std::array<VkDescriptorImageInfo, 5> imageInfos{};

    imageInfos[0] = material.albedoMap->getDescriptorInfo();
    imageInfos[1] = material.normalMap->getDescriptorInfo();
    imageInfos[2] = material.metallicRoughnessMap->getDescriptorInfo();
    imageInfos[3] = material.aoMap->getDescriptorInfo();
    imageInfos[4] = material.emissiveMap->getDescriptorInfo();

    return imageInfos;
}