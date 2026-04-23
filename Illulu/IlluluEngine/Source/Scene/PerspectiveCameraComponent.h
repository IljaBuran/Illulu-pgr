#pragma once

#include "Scene/ICameraComponent.h"
#include "glm/glm.hpp"

struct PerspectiveCameraComponent : ICameraComponent
{
    f32 fovDeg = 75.0f;
    f32 nearPlane = 0.1f;
    f32 farPlane = 180.0f;

    PerspectiveCameraComponent(f32 yaw_ = 0, f32 pitch_ = 0, const glm::vec3& worldUp_ = glm::vec3(0.0f, 1.0f, 0.0f),
        f32 fovDeg_ = 75.0f, f32 nearPlane_ = 0.1f, f32 farPlane_ = 180.0f)
        : ICameraComponent(yaw_, pitch_, worldUp_), fovDeg(fovDeg_), nearPlane(nearPlane_), farPlane(farPlane_) {}

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::PerspectiveCamera;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<PerspectiveCameraComponent>(*this);
    }
    
    glm::mat4 get_projection_matrix(f32 aspect) const override
    {
        return glm::perspective(glm::radians(fovDeg), aspect, nearPlane, farPlane);
    }
};