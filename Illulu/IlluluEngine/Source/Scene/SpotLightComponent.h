#pragma once
#include "Common.h"

#include "Scene/ILightComponent.h"

struct SpotLightComponent : ILightComponent
{
    f32 innerCutoffDeg = 12.5f;
    f32 outerCutoffDeg = 17.5f;

    f32 constant = 1.0f;
    f32 linear = 0.09f;
    f32 quadratic = 0.032f;

    SpotLightComponent() = default;

    SpotLightComponent(const Color& color_, f32 intensity_, f32 innerCutoffDeg_, f32 outerCutoffDeg_,
        f32 constant_ = 1.0f, f32 linear_ = 0.09f, f32 quadratic_ = 0.032f)
    {
        color = color_;
        intensity = intensity_;
        innerCutoffDeg = innerCutoffDeg_;
        outerCutoffDeg = outerCutoffDeg_;
        constant = constant_;
        linear = linear_;
        quadratic = quadratic_;
    }

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::SpotLight;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<SpotLightComponent>(*this);
    }
};