//-----------------------------------------------------------------------------------------
/**
 * \file       ModelDrawable.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a drawable model component with model index and color.
 */
 //-----------------------------------------------------------------------------------------

#pragma once

#include "Common.h"

#include <limits>

#include "Renderer/IDrawable.h"
#include "glm/vec3.hpp"

using Color = glm::vec3;

struct ModelDrawable : IDrawable
{
    u32 modelIdx = std::numeric_limits<u32>::max();
    Color color{1.0f, 1.0f, 1.0f};

    ModelDrawable() = default;
    ModelDrawable(u32 modelIdx_, const Color& color_);

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::DrawableModel;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<ModelDrawable>(*this);
    }

    void draw(Renderer& renderer, const Transform& transform) const override;
};