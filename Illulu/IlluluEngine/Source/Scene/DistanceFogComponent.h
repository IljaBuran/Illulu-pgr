#pragma once

#include "Common.h"

#include "Scene/IComponent.h"

#include <glm/glm.hpp>

enum class DistanceFogType : i32
{
    Off = 0,
    Linear = 1,
    Exp = 2
};

struct DistanceFogComponent : IComponent
{
    DistanceFogType mode = DistanceFogType::Off;

    glm::vec3 color = glm::vec3(0.55f, 0.60f, 0.65f);

    f32 start = 20.0f;
    f32 end = 120.0f;

    f32 density = 0.025f;

    DistanceFogComponent(DistanceFogType mode_ = DistanceFogType::Off, 
        const glm::vec3& color_ = glm::vec3(0.55f, 0.60f, 0.65f),f32 start_ = 20.0f, 
        f32 end_ = 120.0f, f32 density_ = 0.025f)
        : mode(mode_), color(color_), start(start_), end(end_), density(density_) {}

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::DistanceFog;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<DistanceFogComponent>(*this);
    }
};