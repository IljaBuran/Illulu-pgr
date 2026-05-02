#pragma once

#pragma once

#include "Scene/ICameraComponent.h"
#include "glm/glm.hpp"

struct OrthographicCameraComponent : ICameraComponent
{
    f32 orthoHeight;
    f32 nearPlane;
    f32 farPlane;

    OrthographicCameraComponent(      f32        yaw_         = 0.0f, 
                                      f32        pitch_       = 0.0f, 
                                const glm::vec3& worldUp_     = glm::vec3(0.0f, 1.0f, 0.0f),
                                      f32        orthoHeight_ = 10.0f, 
                                      f32        nearPlane_   = -100.0f, 
                                      f32        farPlane_    = 100.0f)
        : ICameraComponent(yaw_, pitch_, worldUp_), 
          orthoHeight(orthoHeight_), 
          nearPlane(nearPlane_), 
          farPlane(farPlane_) {}

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::OrthographicCamera;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<OrthographicCameraComponent>(*this);
    }

    glm::mat4 get_projection_matrix(f32 aspect) const override
    {
        const f32 halfH = orthoHeight * 0.5f;
        const f32 halfW = halfH * aspect;

        return glm::ortho(-halfW, halfW, -halfH, halfH, nearPlane, farPlane);;
    }
};