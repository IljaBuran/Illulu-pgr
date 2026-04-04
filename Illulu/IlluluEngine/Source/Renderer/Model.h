#pragma once

#include "Common.h"

#include "Renderer/Mesh.h"

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

    u32  load_material_diffuse_texture(aiMaterial* material);
    Mesh process_mesh(aiMesh* mesh);
    void process_node(aiNode* node);
};