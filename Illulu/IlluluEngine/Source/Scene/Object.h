#pragma once

#include <memory>
#include <string>
#include <utility>
#include <type_traits>

#include "Scene/Transform.h"
#include "Scene/IComponent.h"

struct Object
{
    std::string name = "unnamed";
    Transform transform;
    std::vector<std::unique_ptr<IComponent>> components;

    Object() = default;

    Object(Object&&) noexcept = default;
    Object& operator=(Object&&) noexcept = default;

    Object(const Object& other);
    Object& operator=(const Object& other);

    template<typename T, typename... Args>
    T& add_component(Args&&... args)
    {
        static_assert(std::is_base_of_v<IComponent, T>, "[OBJECT: add_component]: T must be derived from IComponent");

        components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *static_cast<T*>(components.back().get());
    }

    // todo(ilja): this has no usage rn
    template<typename T>
    T* get_component()
    {
        static_assert(std::is_base_of_v<IComponent, T>, "[OBJECT: get_component]: T must be derived from IComponent");

        for (auto& c : components)
        {
            if (T* typed = dynamic_cast<T*>(c.get()))
                return typed;
        }
        return nullptr;
    }
    
    template<typename T>
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