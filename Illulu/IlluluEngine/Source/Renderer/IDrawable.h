//-----------------------------------------------------------------------------------------
/**
 * \file       IDrawable.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the base interface for drawable scene components.
 */
 //-----------------------------------------------------------------------------------------

#pragma once

#include "Scene/IComponent.h"

struct Renderer;
struct Transform;

struct IDrawable : IComponent
{
    virtual ~IDrawable() = default;

    bool visible = true;

    virtual void draw(Renderer& renderer, const Transform& transform) const = 0;
};