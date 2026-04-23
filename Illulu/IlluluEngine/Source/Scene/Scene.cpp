#include "pch.h"

#include "Scene/Scene.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <memory>

#include "Renderer/IDrawable.h"
#include "Renderer/ModelDrawable.h"
#include "Renderer/SkyboxDrawable.h"
#include "Scene/PerspectiveCameraComponent.h"
#include "Scene/OrthographicCameraComponent.h"
#include "Scene/DirectionalLightComponent.h"
#include "Scene/PointLightComponent.h"
#include "Scene/SpotLightComponent.h"
#include "Core/Window.h"
#include "Core/Debug.h"

void Scene::add_model(Model&& model)
{
    models.push_back(std::move(model));
}

void Scene::remove_model(u32 idx)
{
    models.erase(models.begin() + idx);
}

void Scene::add_object(Object&& object)
{
    objects.push_back(std::move(object));
}

void Scene::rename_object(u32 idx, const std::string& newName)
{
    objects[idx].name = newName;
}

void Scene::duplicate_object(u32 idx)
{
    if (idx >= objects.size())
        return;

    Object copy;
    copy.name = objects[idx].name + "_copy";
    copy.transform = objects[idx].transform;

    std::vector<std::unique_ptr<IComponent>> clonedComponents;
    clonedComponents.reserve(objects[idx].components.size());

    for (const auto& comp : objects[idx].components)
    {
        clonedComponents.push_back(comp->clone());
    }

    add_object(std::move(copy));
    u32 newIdx = static_cast<u32>(objects.size() - 1);

    for (auto& comp : clonedComponents)
    {
        add_object_component(newIdx, std::move(comp));
    }
}

bool Scene::remove_object(u32 idx)
{
    if (idx >= objects.size())
        return false;

    // todo(ilja): temporary solution
    if (idx == currentCamera.objectIdx)
    {
        message_box("Editor", "Can't remove currently active camera").pop_up();
        return false;
    }

    const bool deletedCurrentCameraObject = (currentCamera.objectIdx == idx);

    objects.erase(objects.begin() + idx);

    for (u32 i = 0; i < cachedCameras.size(); i++)
    {
        if (cachedCameras[i].objectIdx == idx)
        {
            cachedCameras.erase(cachedCameras.begin() + i);
            i--;
        }
        else if (cachedCameras[i].objectIdx > idx)
        {
            cachedCameras[i].objectIdx--;
        }
    }

    for (u32 i = 0; i < cachedDrawables.size(); i++)
    {
        if (cachedDrawables[i].objectIdx == idx)
        {
            cachedDrawables.erase(cachedDrawables.begin() + i);
            i--;
        }
        else if (cachedDrawables[i].objectIdx > idx)
        {
            cachedDrawables[i].objectIdx--;
        }
    }
    
    for (u32 i = 0; i < cachedLights.size(); i++)
    {
        if (cachedLights[i].objectIdx == idx)
        {
            cachedLights.erase(cachedLights.begin() + i);
            i--;
        }
        else if (cachedLights[i].objectIdx > idx)
        {
            cachedLights[i].objectIdx--;
        }
    }

    if (deletedCurrentCameraObject)
    {
        if (!cachedCameras.empty())
            currentCamera = cachedCameras[0];
        else
            currentCamera = {};
    }
    else if (currentCamera.objectIdx > idx)
    {
        currentCamera.objectIdx--;
    }
    return true;
}

void Scene::update()
{
    // note(ilja): nothing to update here for now, animations, scripts etc later
}

// todo(ilja): this will be slow for bigger scenes, just redo later in binary format
void Scene::load_from_file(const std::string& path)
{
    std::ifstream ifs(path);
    if (!ifs)
    {
        message_box("Scene::load_from_file", "Invalid path!").pop_up();
        return;
    }

    objects.clear();
    models.clear();
    currentCamera = {};

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
        if (!ifs)
            break;

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

            obj.transform.position = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            obj.transform.rotation = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};
            obj.transform.scale = {std::stof(lGet()), std::stof(lGet()), std::stof(lGet())};

            u32 componentCount = std::stoul(lGet());
            add_object(std::move(obj));
            u32 objectIdx = static_cast<u32>(objects.size() - 1);

            for (u32 i = 0; i < componentCount; i++)
            {
                std::string compToken = lGet();
                if (compToken != "[component]")
                    FATAL_CRASH("[SceneLoader]: Expected [component]");

                ComponentKind kind = component_kind_from_string(lGet());

                switch (kind)
                {
                case ComponentKind::DrawableModel:
                {
                    u32 modelIdx = std::stoul(lGet());
                    Color color = {
                        std::stof(lGet()),
                        std::stof(lGet()),
                        std::stof(lGet())
                    };
                    add_object_component<ModelDrawable>(objectIdx, modelIdx, color);
                    break;
                }

                case ComponentKind::DrawableSkybox:
                {
                    std::array<std::string, 6> faces = {
                        lGet(), lGet(), lGet(), lGet(), lGet(), lGet()
                    };
                    add_object_component<SkyboxDrawable>(objectIdx, faces);
                    break;
                }

                case ComponentKind::PerspectiveCamera:
                {
                    f32 yaw = std::stof(lGet());
                    f32 pitch = std::stof(lGet());
                    glm::vec3 worldUp = {
                        std::stof(lGet()),
                        std::stof(lGet()),
                        std::stof(lGet())
                    };
                    f32 fovDeg = std::stof(lGet());
                    f32 nearPlane = std::stof(lGet());
                    f32 farPlane = std::stof(lGet());
                    add_object_component<PerspectiveCameraComponent>(objectIdx, yaw, pitch, worldUp, 
                        fovDeg, nearPlane, farPlane);
                    break;
                }
                
                case ComponentKind::OrthographicCamera:
                {
                    f32 yaw = std::stof(lGet());
                    f32 pitch = std::stof(lGet());
                    glm::vec3 worldUp = {
                        std::stof(lGet()),
                        std::stof(lGet()),
                        std::stof(lGet())
                    };
                    f32 orthoHeight = std::stof(lGet());
                    f32 nearPlane = std::stof(lGet());
                    f32 farPlane = std::stof(lGet());
                    add_object_component<OrthographicCameraComponent>(objectIdx, yaw, pitch, worldUp, orthoHeight,
                        nearPlane, farPlane);
                    break;
                }


                case ComponentKind::FreeCameraController:
                {
                    FATAL_CRASH("NOT IMPLEMENTED");
                }

                case ComponentKind::DirectionalLight:
                {
                    glm::vec3 color = {
                        std::stof(lGet()),
                        std::stof(lGet()),
                        std::stof(lGet())
                    };

                    f32 intensity = std::stof(lGet());

                    add_object_component<DirectionalLightComponent>(objectIdx, color, intensity);
                    break;
                }

                case ComponentKind::PointLight:
                {
                    glm::vec3 color = {
                        std::stof(lGet()),
                        std::stof(lGet()),
                        std::stof(lGet())
                    };
                    f32 intensity = std::stof(lGet());
                    f32 constant  = std::stof(lGet());
                    f32 linear    = std::stof(lGet());
                    f32 quadratic = std::stof(lGet());

                    add_object_component<PointLightComponent>(objectIdx, color, intensity, constant, linear, quadratic);
                    break;
                }

                case ComponentKind::SpotLight:
                {
                    glm::vec3 color = {
                        std::stof(lGet()),
                        std::stof(lGet()),
                        std::stof(lGet())
                    };
                    f32 intensity      = std::stof(lGet());
                    f32 innerCutoffDeg = std::stof(lGet());
                    f32 outerCutoffDeg = std::stof(lGet());
                    f32 constant       = std::stof(lGet());
                    f32 linear         = std::stof(lGet());
                    f32 quadratic      = std::stof(lGet());
                    
                    add_object_component<SpotLightComponent>(objectIdx, color, intensity, innerCutoffDeg, 
                        outerCutoffDeg, constant, linear, quadratic);
                    break;
                }

                default:
                    FATAL_CRASH("[SceneLoader]: Unsupported component kind");
                }
            }
        }
        else if (token == "[current_camera]")
        {
            currentCamera = {std::stoul(lGet()), std::stoul(lGet())};
        }
        else if (token == "[end]")
        {
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
        message_box("Scene::save_to_file", "Invalid path!").pop_up();
        return;
    }

    ofs << "[scene]\n" << name << "\n\n";

    for (const Model& model : models)
    {
        ofs << "[model]\n";
        ofs << model.sourcePath << "\n\n";
    }

    for (const Object& object : objects)
    {
        ofs << "[object]\n";
        ofs << object.name << '\n';

        ofs << object.transform.position.x << ' '
            << object.transform.position.y << ' '
            << object.transform.position.z << '\n';

        ofs << object.transform.rotation.x << ' '
            << object.transform.rotation.y << ' '
            << object.transform.rotation.z << '\n';

        ofs << object.transform.scale.x << ' '
            << object.transform.scale.y << ' '
            << object.transform.scale.z << '\n';

        ofs << object.components.size() << '\n';

        for (const auto& comp : object.components)
        {
            ofs << "[component]\n";
            ofs << component_kind_to_string(comp->get_component_kind()) << '\n';

            switch (comp->get_component_kind())
            {
                case ComponentKind::DrawableModel:
                {
                    const auto* md = static_cast<const ModelDrawable*>(comp.get());
                    ofs << md->modelIdx << '\n';
                    ofs << md->color.x << ' '
                        << md->color.y << ' '
                        << md->color.z << '\n';
                    break;
                }

                case ComponentKind::DrawableSkybox:
                {
                    const auto* sd = static_cast<const SkyboxDrawable*>(comp.get());
                    for (const std::string& face : sd->faces)
                        ofs << face << '\n';
                    break;
                }

                case ComponentKind::PerspectiveCamera:
                {
                    const auto* cam = static_cast<const PerspectiveCameraComponent*>(comp.get());
                    ofs << cam->yaw << '\n';
                    ofs << cam->pitch << '\n';
                    ofs << cam->worldUp.x << ' '
                        << cam->worldUp.y << ' '
                        << cam->worldUp.z << '\n';
                    ofs << cam->fovDeg << '\n';
                    ofs << cam->nearPlane << '\n';
                    ofs << cam->farPlane << '\n';
                    break;
                }

                case ComponentKind::OrthographicCamera:
                {
                    const auto* cam = static_cast<const OrthographicCameraComponent*>(comp.get());
                    ofs << cam->yaw << '\n';
                    ofs << cam->pitch << '\n';
                    ofs << cam->worldUp.x << ' '
                        << cam->worldUp.y << ' '
                        << cam->worldUp.z << '\n';
                    ofs << cam->orthoHeight << '\n';
                    ofs << cam->nearPlane << '\n';
                    ofs << cam->farPlane << '\n';
                    break;
                }

                case ComponentKind::DirectionalLight:
                {
                    const auto* light = static_cast<const DirectionalLightComponent*>(comp.get());
                    
                    ofs << light->color.x << ' '
                        << light->color.y << ' '
                        << light->color.z << '\n';

                    ofs << light->intensity << '\n';
                    break;
                }

                case ComponentKind::PointLight:
                {
                    const auto* light = static_cast<const PointLightComponent*>(comp.get());
                    ofs << light->color.x << ' '
                        << light->color.y << ' '
                        << light->color.z << '\n';
                    ofs << light->intensity << '\n';
                    ofs << light->constant << '\n';
                    ofs << light->linear << '\n';
                    ofs << light->quadratic << '\n';
                    break;
                }

                case ComponentKind::SpotLight:
                {
                    const auto* light = static_cast<const SpotLightComponent*>(comp.get());
                    ofs << light->color.x << ' '
                        << light->color.y << ' '
                        << light->color.z << '\n';
                    ofs << light->intensity << '\n';
                    ofs << light->innerCutoffDeg << '\n';
                    ofs << light->outerCutoffDeg << '\n';
                    ofs << light->constant << '\n';
                    ofs << light->linear << '\n';
                    ofs << light->quadratic << '\n';
                    break;
                }

                default:
                    FATAL_CRASH("[Scene::save_to_file]: Unsupported component kind");
            }
        }

        ofs << '\n';
    }

    ofs << "[current_camera]\n";
    ofs << currentCamera.objectIdx << ' ' << currentCamera.componentIdx << "\n\n";

    ofs << "[end]\n";
}

Object* Scene::get_current_camera_object()
{
    return &objects[currentCamera.objectIdx];
}

const Object* Scene::get_current_camera_object() const
{
    return &objects[currentCamera.objectIdx];
}

ICameraComponent* Scene::get_current_camera_component()
{
    return reinterpret_cast<ICameraComponent*>(objects[currentCamera.objectIdx].components[currentCamera.componentIdx].get());
}

const ICameraComponent* Scene::get_current_camera_component() const
{
    return reinterpret_cast<const ICameraComponent*>(objects[currentCamera.objectIdx].components[currentCamera.componentIdx].get());
}