#include "pch.h"

#include "Scene/Scene.h"

#include <algorithm>
#include <fstream>
#include <string>

void Scene::add_model(Model&& model)
{
    auto it = std::find_if(models.begin(), models.end(),
        [&](const Model& m) { return m.name == model.name; });

    if (it != models.end())
    {
        Window::message_box("ModelManager", "Model couldn't be added, it is already in the vector");
        return;
    }

    models.push_back(std::move(model));
}

u32 Scene::find_model(const std::string& name) const
{
    for (u32 i = 0; i < models.size(); ++i)
    {
        if (models[i].name == name)
            return i;
    }

    return static_cast<u32>(-1);
}

void Scene::remove_model(const std::string& modelName)
{
    auto it = std::find_if(models.begin(), models.end(),
        [&](const Model& m) { return m.name == modelName; });

    if (it == models.end())
    {
        Window::message_box("ModelManager", "Model couldn't be deleted, it's not in the vector");
        return;
    }

    models.erase(it);
}

void Scene::add_object(Object&& object)
{
    auto it = std::find_if(objects.begin(), objects.end(),
        [&](const Object& o) { return o.name == object.name; });

    if (it != objects.end())
    {
        Window::message_box("ObjectManager", "Object couldn't be added, it is already in the vector");
        return;
    }

    objects.push_back(std::move(object));
}

u32 Scene::find_object(const std::string& name) const
{
    for (u32 i = 0; i < objects.size(); ++i)
    {
        if (objects[i].name == name)
            return i;
    }

    return static_cast<u32>(-1);
}

void Scene::rename_object(u32 idx, const std::string& newName)
{
    objects[idx].name = newName;
}

void Scene::remove_object(const std::string& objectName)
{
    auto it = std::find_if(objects.begin(), objects.end(),
        [&](const Object& o) { return o.name == objectName; });

    if (it == objects.end())
    {
        Window::message_box("ObjectManager", "Object couldn't be deleted, it's not in the vector");
        return;
    }

    objects.erase(it);
}

void Scene::add_camera(Camera&& camera = Camera())
{
    cameras.push_back(std::move(camera));
}

void Scene::rename_camera(u32 camIdx, const std::string& newName)
{
    cameras[currentCameraIdx].name = newName;
}

void Scene::remove_camera(u32 idx)
{
    if (idx >= cameras.size())
    {
        Window::message_box("SceneManager", "Trying to remove camera with invalid idx");
        return;
    }
    if (cameras.size() == 1)
    {
        Window::message_box("SceneManager", "Trying to remove last camera in the scene");
        return;
    }

    if (currentCameraIdx == 0)
        currentCameraIdx++;
    else
        currentCameraIdx--;

    cameras.erase(cameras.begin() + idx);
}



void Scene::update()
{
    // note(ilja): nothing to update here for now, animations, scripts etc later
}

// todo(ilja): this shit will be slow for bigger scenes, just redo later in binary format
void Scene::load_from_file(const std::string& path)
{

    std::ifstream ifs(path);
    if (!ifs)
    {
        Window::message_box("SceneLoader::load_from_file", "Invalid Path!");
        return;
    }

    objects.clear();
    models.clear();
    cameras.clear();
    currentCameraIdx = 0;

    auto lGet = [&]() -> std::string
    {
        std::string s;
        ifs >> s;

        if (!ifs)
            FATAL_CRASH("[SceneLoader]: Unexpected EOF");

        return s;
    };

    while (ifs)
    {
        std::string token;
        ifs >> token;

        if (token == "[scene]")
        {
            name = lGet();
        }
        else if (token == "[model]")
        {
           add_model(Model(ModelLoader(lGet().c_str())));
        }
        else if (token == "[object]")
        {
            Object obj;
            obj.name = lGet();
            obj.modelIdx = std::stoul(lGet());
            obj.transform.position = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            obj.transform.rotation = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            obj.transform.scale = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            obj.color = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};

            add_object(std::move(obj));
        }
        else if (token == "[camera]")
        {
            Camera cam;
            cam.name = lGet();
            cam.position = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            cam.front = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            cam.up = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            cam.right = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            cam.worldUp = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            cam.yaw = std::stof(lGet());
            cam.pitch = std::stof(lGet());

            add_camera(std::move(cam));
        }
        else if (token == "[current_camera]")
        {
            currentCameraIdx = std::stoul(lGet());
        }
        else if (token == "[end]")
        {
            _check_camera();
            return;
        }
        else
        {
            std::string errorMsg = "[SceneLoader]: Invalid token read: \"";
            errorMsg += token;
            errorMsg += "\"";
            FATAL_CRASH(errorMsg.c_str());
        }
    }
    FATAL_CRASH("[SceneLoader]: [end] not found");
}

void Scene::save_to_file(const std::string& path)
{
    std::ofstream ofs(path);
    if (!ofs)
    {
        Window::message_box("SceneLoader::save_to_file", "Invalid Path!");
        return;
    }


    ofs << "[scene]\n" << name << "\n\n";

    for (const Model& model : models)
        ofs << "[model]\n" << model.sourcePath << "\n\n";

    for (const Object& object : objects)
    {
        ofs << "[object]\n"
        << object.name << '\n'
        << object.modelIdx << '\n'
        << object.transform.position.x << ' '
        << object.transform.position.y << ' '
        << object.transform.position.z << '\n'
        << object.transform.rotation.x << ' '
        << object.transform.rotation.y << ' '
        << object.transform.rotation.z << '\n'
        << object.transform.scale.x << ' '
        << object.transform.scale.y << ' '
        << object.transform.scale.z << '\n'
        << object.color.x << ' '
        << object.color.y << ' '
        << object.color.z << "\n\n";
    }

    for (const Camera& camera : cameras)
    {
        ofs << "[camera]\n"
            << camera.name << '\n'
            << camera.position.x << ' '
            << camera.position.y << ' '
            << camera.position.z << '\n'
            << camera.front.x << ' '
            << camera.front.y << ' '
            << camera.front.z << '\n'
            << camera.up.x << ' '
            << camera.up.y << ' '
            << camera.up.z << '\n'
            << camera.right.x << ' '
            << camera.right.y << ' '
            << camera.right.z << '\n'
            << camera.worldUp.x << ' '
            << camera.worldUp.y << ' '
            << camera.worldUp.z << '\n'
            << camera.yaw << '\n'
            << camera.pitch << "\n\n";
    }

    ofs << "[current_camera]\n" << currentCameraIdx << "\n\n";
    ofs << "[end]\n";

    Window::message_box("SceneLoader", "Saved successfully");
}

void Scene::_check_camera()
{
    if (!cameras.empty())
        return;

    Window::message_box("SceneManager", "Camera not found in scene, creating default one!");

    cameras.emplace_back(Camera());
    currentCameraIdx = 0;
}
