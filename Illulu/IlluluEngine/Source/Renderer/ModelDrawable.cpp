#include "pch.h"
#include "ModelDrawable.h"
#include "Renderer/Renderer.h"

ModelDrawable::ModelDrawable(u32 modelIdx_, const Color& color_)
    : modelIdx(modelIdx_), color(color_)
{}

void ModelDrawable::draw(Renderer& renderer, const Transform& transform) const
{
    renderer.draw_model(modelIdx, color, transform);
}
