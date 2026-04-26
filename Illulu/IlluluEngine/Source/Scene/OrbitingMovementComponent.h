#pragma once

#include "Common.h"

#include "Scene/IComponent.h"

#include <glm/glm.hpp>

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
    f32 speed; // note(ilja): probabiliy in radians?
    f32 angle;

    bool lookAtCenter;
    OrbitingAxis orbitingAxis = OrbitingAxis::Y;

    OrbitingMovementComponent(const glm::vec3& center_ = {0.0f, 0.0f, 0.0f}, f32 radius_ = 5.0f, f32 height_ = 0.0f,
        f32 speed_ = 1.0f, f32 angle_ = 0.0f, bool lookAtCenter_ = false, OrbitingAxis orbitingAxis_ = OrbitingAxis::Y)
        : center(center_), radius(radius_), height(height_), speed(speed_), angle(angle_), lookAtCenter(lookAtCenter_), 
          orbitingAxis(orbitingAxis_) {}

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::OrbitingMovement;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<OrbitingMovementComponent>(*this);
    }
};