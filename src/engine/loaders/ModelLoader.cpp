#include "ModelLoader.h"
#include <magic_enum.hpp>

void EngineModelLoader::ProcessNode(aiNode *node, const aiScene *scene, Entity &entity)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, entity);
    }
    
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, entity);
    }
}

void EngineModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene, Entity &entity)
{
    MeshComponent& meshComponent = entity.AddOrGetComponent<MeshComponent>();
    MaterialComponent& materialComponent = entity.AddOrGetComponent<MaterialComponent>();
    RenderComponent& renderComponent = entity.AddOrGetComponent<RenderComponent>();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if(mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        if(mesh->mTextureCoords[0]) {
            vertex.texCoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
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

    renderComponent.material = materialComponent;
    renderComponent.mesh = meshComponent;
    renderComponent.name = mesh->mName.C_Str();
}

void EngineModelLoader::LoadMaterialTextures(aiMaterial *mat, Entity& entity)
{
    MaterialComponent& materialComponent = entity.AddOrGetComponent<MaterialComponent>();
    aiString texturePath;
    aiColor3D color;

    // Array of texture types to process
    std::vector<aiTextureType> textureTypes = {
        aiTextureType_DIFFUSE,
        aiTextureType_SPECULAR,
        aiTextureType_AMBIENT,
        aiTextureType_EMISSIVE,
        aiTextureType_HEIGHT,
        aiTextureType_NORMALS,
        aiTextureType_SHININESS,
        aiTextureType_OPACITY,
        aiTextureType_DISPLACEMENT,
        aiTextureType_LIGHTMAP,
        aiTextureType_REFLECTION,
        aiTextureType_BASE_COLOR,
        aiTextureType_NORMAL_CAMERA,
        aiTextureType_EMISSION_COLOR,
        aiTextureType_METALNESS,
        aiTextureType_DIFFUSE_ROUGHNESS,
        aiTextureType_AMBIENT_OCCLUSION
    };

    for(const auto& type : textureTypes) {
        if(mat->GetTexture(type, 0, &texturePath) == AI_SUCCESS) {
            std::cout << "Found: " << magic_enum::enum_name(type) << std::endl;
            const aiScene* scene = importer.GetScene();
            const aiTexture* texture = scene->GetEmbeddedTexture(texturePath.C_Str());

            VkImage textureImage;
            VkDeviceMemory textureImageMemory;

            if (texture) {
                if (texture->mHeight == 0) {
                    vulkanRenderer.getCore()->createTextureImage(
                        reinterpret_cast<const char*>(texture->pcData),
                        textureImage,
                        textureImageMemory
                    );
                    // Log texture creation from embedded texture
                    std::cout << "Created texture from embedded texture." << std::endl;
                }
            } else {
                std::string fullPath = directory + '/' + texturePath.C_Str();
                std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
                vulkanRenderer.getCore()->createTextureImage(
                    fullPath.c_str(),
                    textureImage,
                    textureImageMemory
                );
                // Log texture creation from file
                std::cout << "Created texture from file: " << fullPath << std::endl;
            }

            VkImageView textureImageView = vulkanRenderer.getCore()->createImageView(
                textureImage,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_ASPECT_COLOR_BIT
            );

            // Store texture information based on type
            switch(type) {
                case aiTextureType_DIFFUSE:
                    materialComponent.diffuseImage = textureImage;
                    materialComponent.diffuseImageMemory = textureImageMemory;
                    materialComponent.diffuseImageView = textureImageView;
                    // Log diffuse texture assignment
                    std::cout << "Assigned diffuse texture." << std::endl;
                    break;
                case aiTextureType_SPECULAR:
                    materialComponent.specularImage = textureImage;
                    materialComponent.specularImageMemory = textureImageMemory;
                    materialComponent.specularImageView = textureImageView;
                    // Log specular texture assignment
                    std::cout << "Assigned specular texture." << std::endl;
                    break;
                case aiTextureType_NORMALS:
                    materialComponent.normalImage = textureImage;
                    materialComponent.normalImageMemory = textureImageMemory;
                    materialComponent.normalImageView = textureImageView;
                    // Log normal texture assignment
                    std::cout << "Assigned normal texture." << std::endl;
                    break;
                // Add other texture types as needed
            }
        }
    }

    // Set material colors and properties
    if(mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        materialComponent.diffuse = glm::vec3(color.r, color.g, color.b);
    }

    if(mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        materialComponent.specular = glm::vec3(color.r, color.g, color.b);
    }

    if(mat->Get(AI_MATKEY_SHININESS, materialComponent.shininess) == AI_SUCCESS) {
        materialComponent.shininess = std::max(materialComponent.shininess, 1.0f);
    }
}

bool EngineModelLoader::LoadModel(const std::string &path, Entity &entity)
{
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene *scene = importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_GenNormals | 
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }

    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene, entity);

    if(scene->HasMaterials()) {
        LoadMaterialTextures(scene->mMaterials[0], entity);
    }

    return true;
}