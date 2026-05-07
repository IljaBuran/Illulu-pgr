#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       FloatingTextureComponent.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a component for smoothly moving texture coordinates over time.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"
#include "Scene/IComponent.h"

#include <glm/vec2.hpp>

struct FloatingTextureComponent : IComponent
{
    glm::vec2 speed;
    glm::vec2 offset;

    bool playing;
    bool loop;

    FloatingTextureComponent(
        glm::vec2 speed_ = glm::vec2(0.1f, 0.0f),
        bool playing_ = true,
        bool loop_ = true
    )
        : speed(speed_),
          offset({0.0f, 0.0f}),
          playing(playing_),
          loop(loop_)
    {
    }

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::FloatingTexture;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<FloatingTextureComponent>(*this);
    }
};