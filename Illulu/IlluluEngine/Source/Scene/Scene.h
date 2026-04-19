#pragma once

#include <vector>
#include <string>
#include <array>
#include <utility>
#include <type_traits>
#include <memory>
#include <numeric>

#include "Scene/Object.h"
#include "Scene/ICameraComponent.h"
#include "Scene/PerspectiveCameraComponent.h"
#include "Scene/IComponent.h"
#include "Scene/ILightComponent.h"
#include "Renderer/IDrawable.h"
#include "Renderer/Model.h"

struct Scene
{
    std::string name = "Scene";
    std::vector<Object> objects;
    std::vector<Model>  models;
    
    ComponentRef currentCamera{};

    std::vector<ComponentRef> cachedCameras;
    std::vector<ComponentRef> cachedDrawables;
    std::vector<ComponentRef> cachedLights;

    std::vector<ComponentRef> transparentDrawables;

    void add_model(Model&& model);
    void remove_model(u32 idx);

    void add_object(Object&& object);
    void rename_object(u32 idx, const std::string& newName);
    void duplicate_object(u32 idx);
    bool remove_object(u32 idx);
    
    template<typename T, typename... Args>
    T& add_object_component(u32 idx, Args&&... args)
    {
        if (idx >= objects.size())
            FATAL_CRASH("[SCENE: add_object_component]: invalid object index");
        
        static_assert(std::is_base_of_v<IComponent, T>,
            "[SCENE: add_object_component]: T must be derived from IComponent");

        objects[idx].components.push_back(std::make_unique<T>(std::forward<Args>(args)...));

        u32 compIdx = static_cast<u32>(objects[idx].components.size() - 1);

        if constexpr (std::is_base_of_v<ICameraComponent, T>)
            cachedCameras.emplace_back(ComponentRef{idx, compIdx});

        if constexpr (std::is_base_of_v<IDrawable, T>)
            cachedDrawables.emplace_back(ComponentRef{idx, compIdx});

        if constexpr (std::is_base_of_v<ILightComponent, T>)
            cachedLights.emplace_back(ComponentRef{idx, compIdx});

        return *static_cast<T*>(objects[idx].components.back().get());
    }
    
    IComponent& add_object_component(u32 idx, std::unique_ptr<IComponent> comp)
    {
        if (idx >= objects.size())
            FATAL_CRASH("[SCENE: add_object_component]: invalid object index");

        if (!comp)
            FATAL_CRASH("[SCENE: add_object_component]: null component");

        objects[idx].components.push_back(std::move(comp));
        u32 compIdx = static_cast<u32>(objects[idx].components.size() - 1);

        IComponent* raw = objects[idx].components.back().get();

        if (dynamic_cast<ICameraComponent*>(raw))
            cachedCameras.emplace_back(ComponentRef{idx, compIdx});

        if (dynamic_cast<IDrawable*>(raw))
            cachedDrawables.emplace_back(ComponentRef{idx, compIdx});
        
        if (dynamic_cast<ILightComponent*>(raw))
            cachedLights.emplace_back(ComponentRef{idx, compIdx});

        return *raw;
    }

    void update();

    void load_from_file(const std::string& path);
    void save_to_file(const std::string& path);

    Object* get_current_camera_object();
    const Object* get_current_camera_object() const;

    ICameraComponent* get_current_camera_component();
    const ICameraComponent* get_current_camera_component() const;
};