#include "ModelLoader.h"

void EngineModelLoader::ProcessNode(aiNode *node, const aiScene *scene, MeshComponent &meshComponent, MaterialComponent& materialComponent)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, meshComponent, materialComponent);
    }
    
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, meshComponent, materialComponent);
    }
}

void EngineModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene, MeshComponent &meshComponent, MaterialComponent& materialComponent)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};

        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if(mesh->HasNormals())
        {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        if(mesh->mTextureCoords[0])
        {
            vertex.texCoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    vulkanRenderer.getCore()->createBuffer(
        sizeof(vertices[0]) * vertices.size(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.vertexBuffer,
        meshComponent.vertexBufferMemory
    );

    vulkanRenderer.getCore()->createBuffer(
        sizeof(indices[0]) * indices.size(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        meshComponent.indexBuffer,
        meshComponent.indexBufferMemory
    );

    vulkanRenderer.getCore()->copyDataToBuffer(vertices.data(), meshComponent.vertexBufferMemory, sizeof(vertices[0]) * vertices.size());
    vulkanRenderer.getCore()->copyDataToBuffer(indices.data(), meshComponent.indexBufferMemory, sizeof(indices[0]) * indices.size());

    meshComponent.indexCount = static_cast<uint32_t>(indices.size());
}

void EngineModelLoader::LoadMaterialTextures(aiMaterial *mat, MaterialComponent &materialComponent)
{
    aiString texturePath;
    aiColor3D color;

    // Load diffuse properties
    if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
    {
        std::string fullPath = directory + '/' + texturePath.C_Str();
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        // Create texture image
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        vulkanRenderer.getCore()->createTextureImage(
            fullPath.c_str(),
            textureImage,
            textureImageMemory
        );

        // Create texture image view
        VkImageView textureImageView = vulkanRenderer.getCore()->createImageView(
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT
        );


        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        vulkanRenderer.getCore()->createBuffer(
            sizeof(UBO),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniformBuffer,
            uniformBufferMemory
        );
        
        materialComponent.pipeline = vulkanRenderer.getCore()->getPipeline()->getPipeline();
        materialComponent.pipelineLayout = vulkanRenderer.getCore()->getPipeline()->getLayout();
        materialComponent.descriptorSet = vulkanRenderer.getCore()->getDescriptor()->getDescriptorSet(vulkanRenderer.getCore()->getCurrentFrame());
        materialComponent.uniformBuffer = uniformBuffer;
        materialComponent.uniformBufferMemory = uniformBufferMemory;

        // Store in material component
        materialComponent.textureImage = textureImage;
        materialComponent.textureImageMemory = textureImageMemory;
        materialComponent.textureImageView = textureImageView;
    }

    if(mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        materialComponent.diffuse = glm::vec3(color.r, color.g, color.b);
    }

    // Load specular properties
    if(mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        materialComponent.specular = glm::vec3(color.r, color.g, color.b);
    }

    // Load shininess
    float shininess;
    if(mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
    {
        materialComponent.shininess = shininess;
    }
}

bool EngineModelLoader::LoadModel(const std::string &path, Entity &entity)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_GenNormals | 
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        return false;
    }

    directory = path.substr(0, path.find_last_of('/'));

    auto& meshComponent = entity.addComponent<MeshComponent>();
    auto& materialComponent = entity.addComponent<MaterialComponent>();
    auto& renderComponent = entity.addComponent<RenderComponent>();
    renderComponent.material = materialComponent;
    renderComponent.mesh = meshComponent;
    
    ProcessNode(scene->mRootNode, scene, meshComponent, materialComponent);

    if(scene->HasMaterials())
        LoadMaterialTextures(scene->mMaterials[0], materialComponent);

    return true;
}