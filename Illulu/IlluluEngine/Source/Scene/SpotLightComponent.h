#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       SpotLightComponent.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a spotlight component with cutoff angles and attenuation settings.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"

#include "Scene/ILightComponent.h"

struct SpotLightComponent : ILightComponent
{
    f32 innerCutoffDeg;
    f32 outerCutoffDeg;

    f32 constant;
    f32 linear;
    f32 quadratic;

    SpotLightComponent(
        const Color& color_ = Color{ 1.0f, 1.0f, 1.0f },
        f32 intensity_ = 1.0f,
        f32 innerCutoffDeg_ = 12.5f,
        f32 outerCutoffDeg_ = 17.5f,
        f32 constant_ = 1.0f,
        f32 linear_ = 0.09f,
        f32 quadratic_ = 0.032f
    )
        : ILightComponent(color_, intensity_),
        innerCutoffDeg(innerCutoffDeg_),
        outerCutoffDeg(outerCutoffDeg_),
        constant(constant_),
        linear(linear_),
        quadratic(quadratic_)
    {
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