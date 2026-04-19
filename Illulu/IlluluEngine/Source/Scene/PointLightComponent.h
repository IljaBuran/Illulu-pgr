#pragma once
#include "Common.h"
#include "Scene/ILightComponent.h"

struct PointLightComponent : ILightComponent
{
    f32 constant = 1.0f;
    f32 linear = 0.09f;
    f32 quadratic = 0.032f;

    PointLightComponent() = default;

    PointLightComponent(const glm::vec3& color_, f32 intensity_,
        f32 constant_  = 1.0f,
        f32 linear_    = 0.09f,
        f32 quadratic_ = 0.032f)
    {
        color = color_;
        intensity = intensity_;
        constant = constant_;
        linear = linear_;
        quadratic = quadratic_;
    }

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::PointLight;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<PointLightComponent>(*this);
    }
};