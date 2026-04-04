#include "pch.h"

#include <filesystem>

#include "Core/Filesystem.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"

#include <assimp/postprocess.h>


/* Model */
Model::Model(ModelLoader&& md)
{
    std::filesystem::path p(md.path);

    sourcePath = md.path;
    name = (p.parent_path().filename() / p.filename()).generic_string();
    meshes = std::move(md.meshes);
}

/* ModelLoader */
ModelLoader::ModelLoader(const char* modelPath)
    : path(modelPath)
{
    scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);
    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        FATAL_CRASH(importer.GetErrorString())

    process_node(scene->mRootNode);
}

u32 ModelLoader::load_material_diffuse_texture(aiMaterial* material)
{
    if (!material->GetTextureCount(aiTextureType_DIFFUSE))
        return 0;

    aiString aiPath;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath) != aiReturn_SUCCESS)
        return 0;

    std::string fullPath = (std::filesystem::path(get_directory(path)) / aiPath.C_Str()).generic_string();

    auto it = textureCache.find(fullPath);
    if (it != textureCache.end())
        return it->second;

    u32 texture = load_texture_2d(fullPath);
    textureCache[fullPath] = texture;
    return texture;
}

Mesh ModelLoader::process_mesh(aiMesh* mesh)
{
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    vertices.reserve(mesh->mNumVertices);

    for (u32 i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.uv = { 0.0f, 0.0f };
        }

        vertices.push_back(vertex);
    }

    for (u32 i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        for (u32 j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    u32 diffuseTexture = 0;
    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        diffuseTexture = load_material_diffuse_texture(material);
    }

    return Mesh(vertices, indices, diffuseTexture);
}

void ModelLoader::process_node(aiNode* node)
{
    for (u32 i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh));
    }

    for (u32 i = 0; i < node->mNumChildren; i++)
        process_node(node->mChildren[i]);
}