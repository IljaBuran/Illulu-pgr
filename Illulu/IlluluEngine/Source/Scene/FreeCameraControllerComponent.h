#pragma once

#include "Common.h"
#include "Scene/IComponent.h"

struct FreeCameraControllerComponent : IComponent
{
    f32  movementSpeed = 9.0f;
    f32  mouseSenitivity = 0.1f;
    bool enabled = true;

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::FreeCameraController;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<FreeCameraControllerComponent>(*this);
    }
};