#pragma once

#include <vector>
#include <string>

#include "Scene/Object.h"
#include "Scene/Camera.h"
#include "Renderer/Model.h"

struct Scene
{
    std::string name = "Scene";
    std::vector<Object> objects;
    std::vector<Model>  models;
    std::vector<Camera> cameras = { Camera() };
    u32 currentCameraIdx = 0;

    void add_model(Model&& model);
    u32  find_model(const std::string& name) const;
    void remove_model(const std::string& modelName);

    void add_object(Object&& object);
    u32  find_object(const std::string& name) const;
    void rename_object(u32 idx, const std::string& newName);
    void remove_object(const std::string& objectName);

    void add_camera(Camera&& camera);
    void rename_camera(u32 camIdx, const std::string& newName);
    void remove_camera(u32 idx);

    void update();

    void load_from_file(const std::string& path);
    void save_to_file(const std::string& path);

    void _check_camera();
};