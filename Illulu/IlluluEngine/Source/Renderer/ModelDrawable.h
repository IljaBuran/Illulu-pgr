#pragma once

#include "Renderer/IDrawable.h"
#include "glm/vec3.hpp"
#include "Common.h"

using Color = glm::vec3;

struct ModelDrawable : IDrawable
{
    u32 modelIdx = static_cast<u32>(-1);
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