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
#include "Scene/CollisionBoxComponent.h"
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
    std::vector<ComponentRef> cachedCollisionBoxes;

    void add_model(Model&& model);
    void remove_model(u32 idx);

    void add_object(Object&& object);
    bool rename_object(u32 idx, const std::string& newName);
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

        if constexpr (std::is_base_of_v<CollisionBoxComponent, T>)
            cachedCollisionBoxes.emplace_back(ComponentRef{idx, compIdx});


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

        if (dynamic_cast<CollisionBoxComponent*>(raw))
            cachedCollisionBoxes.emplace_back(ComponentRef{idx, compIdx});

        return *raw;
    }

    void delete_object_component(u32 objIdx, u32 compIdx)
    {
        if (objIdx >= objects.size())
        {
            message_box("Warning", "[delete_object_component]: invalid objIdx").pop_up();
            return;
        }
        Object& obj = objects[objIdx];
        std::vector<std::unique_ptr<IComponent>>& comps = obj.components;
        
        if (compIdx >= comps.size())
        {
            message_box("Warning", "[delete_object_component]: invalid compIdx").pop_up();
            return;
        }

        IComponent* raw = comps[compIdx].get();
        if (dynamic_cast<ICameraComponent*>(raw))
        {
            std::erase(cachedCameras, ComponentRef{objIdx, compIdx});
        }

        if (dynamic_cast<IDrawable*>(raw))
        {
            std::erase(cachedDrawables, ComponentRef{objIdx, compIdx});
        }

        if (dynamic_cast<ILightComponent*>(raw))
        {
            std::erase(cachedLights, ComponentRef{objIdx, compIdx});
        }

        if (dynamic_cast<CollisionBoxComponent*>(raw))
        {
            std::erase(cachedCollisionBoxes, ComponentRef{objIdx, compIdx});
        }

        // note(ilja): repair invalidated cache structures
        for (auto& comp : cachedCameras)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedDrawables)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedCameras)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedCollisionBoxes)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        
        comps.erase(comps.begin() + compIdx);
    }

    void update();

    void load_from_file(const std::string& path);
    void save_to_file(const std::string& path);

    Object* get_current_camera_object();
    const Object* get_current_camera_object() const;

    ICameraComponent* get_current_camera_component();
    const ICameraComponent* get_current_camera_component() const;

    void rotate_all_objects_around_origin(const glm::vec3& rotationDeltaDeg);
};