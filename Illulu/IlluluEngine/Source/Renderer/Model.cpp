#include "pch.h"

#include <filesystem>

#include "Core/Filesystem.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include "stb/stb_image.h"

#include <assimp/postprocess.h>

Model::Model(ModelLoader&& md)
{
    std::filesystem::path p(md.path);

    sourcePath = md.path;
    name = (p.parent_path().filename() / p.filename()).generic_string();
    meshes = std::move(md.meshes);
}

ModelLoader::ModelLoader(const char* modelPath)
    : path(modelPath)
{
    scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices);
    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        FATAL_CRASH(importer.GetErrorString())

    process_node(scene->mRootNode);
}

bool ModelLoader::image_has_alpha(const std::string& path)
{
    int w = 0, h = 0, channels = 0;
    if (!stbi_info(path.c_str(), &w, &h, &channels))
        return false;

    return channels == 2 || channels == 4;
}

std::string ModelLoader::get_material_texture_path(const std::string& modelPath, aiMaterial* material, aiTextureType type)
{
    if (!material->GetTextureCount(type))
        return {};

    aiString aiPath;
    if (material->GetTexture(type, 0, &aiPath) != aiReturn_SUCCESS)
        return {};

    return (std::filesystem::path(get_directory(modelPath)) / aiPath.C_Str()).generic_string();
}

u32 ModelLoader::load_material_texture(aiMaterial* material, aiTextureType type)
{
    if (!material->GetTextureCount(type))
        return 0;

    aiString aiPath;
    if (material->GetTexture(type, 0, &aiPath) != aiReturn_SUCCESS)
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

    Material material{};
    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        material = load_material(aiMat);
    }

    return Mesh(vertices, indices, material);
}

Material ModelLoader::load_material(aiMaterial* material)
{
    Material out{};

    out.diffuseTex = load_material_texture(material, aiTextureType_DIFFUSE);
    out.specularTex = load_material_texture(material, aiTextureType_SPECULAR);

    aiColor3D diffuse(1.0f, 1.0f, 1.0f);
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == aiReturn_SUCCESS)
        out.diffuseCol = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

    aiColor3D specular(0.25f, 0.25f, 0.25f);
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular) == aiReturn_SUCCESS)
        out.specularCol = glm::vec3(specular.r, specular.g, specular.b);

    f32 shininess = 32.0f;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS)
        out.shininess = shininess;
    else
        out.shininess = 32.0f;

    out.blendMode = BlendMode::Opaque;

    // explicit opacity => true transparency
    f32 opacity = 1.0f;
    const bool hasExplicitOpacity =
        (material->Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS);

    if (hasExplicitOpacity && opacity < 0.999f)
        out.blendMode = BlendMode::Transparent;

    aiBlendMode aiBlend = aiBlendMode_Default;
    if (material->Get(AI_MATKEY_BLEND_FUNC, aiBlend) & aiReturn_SUCCESS)
    {
        if (aiBlend & aiBlendMode_Additive)
            out.blendMode = BlendMode::Transparent;
    }

    if (out.blendMode == BlendMode::Opaque)
    {
        std::string diffusePath = get_material_texture_path(path, material, aiTextureType_DIFFUSE);
        if (!diffusePath.empty() && image_has_alpha(diffusePath))
        {
            out.blendMode = BlendMode::Masked;
        }
    }

    return out;
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