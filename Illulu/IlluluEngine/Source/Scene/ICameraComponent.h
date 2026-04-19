#pragma once

#include "Scene/IComponent.h"
#include "glm/mat4x4.hpp"

struct Transform;

struct ICameraComponent : IComponent
{
    virtual ~ICameraComponent() = default;

    virtual glm::mat4 get_view_matrix(const Transform& transform) const = 0;
};