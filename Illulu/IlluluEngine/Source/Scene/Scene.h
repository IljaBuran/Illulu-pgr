#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Scene.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the scene structure, object storage, model storage, and component caches.
 */
 //-----------------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <array>
#include <utility>
#include <memory>
#include <concepts>

#include "Scene/Object.h"
#include "Scene/ICameraComponent.h"
#include "Scene/PerspectiveCameraComponent.h"
#include "Scene/IComponent.h"
#include "Scene/ILightComponent.h"
#include "Scene/CollisionBoxComponent.h"
#include "Scene/OrbitingMovementComponent.h"
#include "Scene/AnimatedTextureComponent.h"
#include "Scene/FloatingTextureComponent.h"

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

    std::vector<ComponentRef> cachedOrbiting;
    std::vector<ComponentRef> cachedAnimatedTex;
    std::vector<ComponentRef> cachedFloatingTex;
    
    std::vector<std::pair<std::string, std::string>> currentCollisions;

    void add_model(Model&& model);
    void remove_model(u32 idx);

    void add_object(Object&& object);
    bool rename_object(u32 idx, const std::string& newName);
    void duplicate_object(u32 idx);
    bool remove_object(u32 idx);

    void update_orbit_movement(f32 dt);
    void update_collisions();
    

    // note(ilja): 2 add_object_component overloads, compile time check for valid IComponent parameter
    template<typename T, typename... Args>
    requires std::derived_from<T, IComponent>
    T& add_object_component(u32 idx, Args&&... args)
    {
        if (idx >= objects.size())
            FATAL_CRASH("[SCENE: add_object_component]: invalid object index");
        
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

        if constexpr (std::is_base_of_v<OrbitingMovementComponent, T>)
            cachedOrbiting.emplace_back(ComponentRef{idx, compIdx});
        
        if constexpr (std::is_base_of_v<AnimatedTextureComponent, T>)
            cachedAnimatedTex.emplace_back(ComponentRef{idx, compIdx});

        if constexpr (std::is_base_of_v<FloatingTextureComponent, T>)
            cachedFloatingTex.emplace_back(ComponentRef{ idx, compIdx });

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

        if (dynamic_cast<OrbitingMovementComponent*>(raw))
            cachedOrbiting.emplace_back(ComponentRef{idx, compIdx});

        if (dynamic_cast<AnimatedTextureComponent*>(raw))
            cachedAnimatedTex.emplace_back(ComponentRef{idx, compIdx});

        if (dynamic_cast<FloatingTextureComponent*>(raw))
            cachedFloatingTex.emplace_back(ComponentRef{ idx, compIdx });

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
        if (dynamic_cast<OrbitingMovementComponent*>(raw))
        {
            std::erase(cachedOrbiting, ComponentRef{objIdx, compIdx});
        }
        if (dynamic_cast<AnimatedTextureComponent*>(raw))
        {
            std::erase(cachedAnimatedTex, ComponentRef{objIdx, compIdx});
        }
        if (dynamic_cast<FloatingTextureComponent*>(raw))
        {
            std::erase(cachedFloatingTex, ComponentRef{ objIdx, compIdx });
        }

        // note(ilja): naively repair invalidated cache structures
            // this should be ok, as removing component is not as often
        for (auto& comp : cachedCameras)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedDrawables)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedLights)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedCollisionBoxes)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedOrbiting)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedAnimatedTex)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        for (auto& comp : cachedFloatingTex)
            if (comp.objectIdx == objIdx)
                if (comp.componentIdx > compIdx)
                    comp.componentIdx--;
        
        comps.erase(comps.begin() + compIdx);
    }

    void update(f32 dt);

    void load_from_file(const std::string& path);
    void save_to_file(const std::string& path);

    Object* get_current_camera_object();
    const Object* get_current_camera_object() const;

    ICameraComponent* get_current_camera_component();
    const ICameraComponent* get_current_camera_component() const;

    void rotate_all_objects_around_origin(const glm::vec3& rotationDeltaDeg);
    void move_object_to_camera_location(u32 objIdx);
    void move_object(u32 objIdx, glm::vec3 pos);
};