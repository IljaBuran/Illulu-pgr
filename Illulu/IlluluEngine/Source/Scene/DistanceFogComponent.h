#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       DistanceFogComponent.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a distance fog component with linear and exponential fog settings.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"

#include "Scene/IComponent.h"

#include <glm/glm.hpp>

enum class DistanceFogType : i32
{
    Off = 0,
    Linear,
    Exp
};

struct DistanceFogComponent : IComponent
{
    DistanceFogType mode;

    glm::vec3 color;

    f32 start;
    f32 end;

    f32 density;

    DistanceFogComponent(
        DistanceFogType mode_   = DistanceFogType::Off,
        const glm::vec3& color_ = glm::vec3(0.50f, 0.50f, 0.50f),
        f32 start_              = 20.0f,
        f32 end_                = 120.0f,
        f32 density_            = 0.02f
    )
        : mode(mode_),
        color(color_),
        start(start_),
        end(end_),
        density(density_)
    {
    }

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::DistanceFog;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<DistanceFogComponent>(*this);
    }
};