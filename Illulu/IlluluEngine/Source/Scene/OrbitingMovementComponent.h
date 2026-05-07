#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       OrbitingMovementComponent.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a component for moving objects around a center point on a selected axis.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"

#include "Scene/IComponent.h"

#include <glm/glm.hpp>

/**
 * \brief Defines the axis around which an object should orbit.
 */
enum class OrbitingAxis : u8
{
    X = 0,
    Y,
    Z
};

struct OrbitingMovementComponent : IComponent
{
    glm::vec3 center;

    f32 radius;
    f32 height;
    f32 speed;
    f32 angle;

    bool lookAtCenter;
    OrbitingAxis orbitingAxis = OrbitingAxis::Y;

    OrbitingMovementComponent(
        const glm::vec3& center_ = glm::vec3(0.0f),
        f32 radius_ = 5.0f,
        f32 height_ = 0.0f,
        f32 speed_ = 1.0f,
        f32 angle_ = 0.0f,
        bool lookAtCenter_ = false,
        OrbitingAxis orbitingAxis_ = OrbitingAxis::Y
    )
        : center(center_),
        radius(radius_),
        height(height_),
        speed(speed_),
        angle(angle_),
        lookAtCenter(lookAtCenter_),
        orbitingAxis(orbitingAxis_)
    {
    }

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::OrbitingMovement;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<OrbitingMovementComponent>(*this);
    }
};