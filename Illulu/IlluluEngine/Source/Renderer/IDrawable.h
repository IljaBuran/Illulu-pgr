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