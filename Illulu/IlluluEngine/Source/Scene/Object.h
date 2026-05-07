#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Object.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a scene object with a transform and a list of attached components.
 */
 //-----------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <utility>
#include <type_traits>
#include <vector>

#include "Scene/Transform.h"
#include "Scene/IComponent.h"

struct Object
{
    std::string name = "unnamed";
    Transform transform;
    std::vector<std::unique_ptr<IComponent>> components;

    Object() = default;

    // note(ilja): this needs to stay here, some std::vector operations need it
    Object(Object&&)                       = default;
    Object& operator=(Object&&)            = default;

    Object(const Object& other)            = delete;
    Object& operator=(const Object& other) = delete;

    template<typename T, typename... Args>
    requires std::derived_from<T, IComponent>
    T& add_component(Args&&... args)
    {
        components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *static_cast<T*>(components.back().get());
    }

    // todo(ilja): this has no usage rn
    template<typename T>
    requires std::derived_from<T, IComponent>
    T* get_component()
    {
        for (auto& c : components)
        {
            if (T* typed = dynamic_cast<T*>(c.get()))
                return typed;
        }
        return nullptr;
    }
    
    template<typename T>
    requires std::derived_from<T, IComponent>
    const T* get_component() const
    {
        static_assert(std::is_base_of_v<IComponent, T>, "[OBJECT: get_component const]: T must be derived from IComponent");

        for (const auto& c : components)
        {
            if (const T* typed = dynamic_cast<const T*>(c.get()))
                return typed;
        }
        return nullptr;
    }
};