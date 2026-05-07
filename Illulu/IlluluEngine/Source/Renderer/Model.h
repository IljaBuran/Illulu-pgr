//-----------------------------------------------------------------------------------------
/**
 * \file       Model.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines model loading structures for importing meshes and materials with Assimp.
 */
 //-----------------------------------------------------------------------------------------

#pragma once

#include "Common.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>


#include <unordered_map>
#include <string>

struct ModelLoader;

struct Model
{
    std::vector<Mesh> meshes;
    std::string       name;
    std::string       sourcePath;

    Model(ModelLoader&& md);
};


struct ModelLoader
{
    ModelLoader(const char* modelPath);

    const aiScene*                       scene;
    std::string                          path;
    std::unordered_map<std::string, u32> textureCache;
    std::vector<Mesh>                    meshes;
    Assimp::Importer                     importer;

private:

    static bool image_has_alpha(const std::string& path);
    static std::string get_material_texture_path(const std::string& modelPath, aiMaterial* material, aiTextureType type);

    u32  load_material_texture(aiMaterial* material, aiTextureType type);
    Mesh process_mesh(aiMesh* mesh);
    Material load_material(aiMaterial* material);
    void process_node(aiNode* node);
};