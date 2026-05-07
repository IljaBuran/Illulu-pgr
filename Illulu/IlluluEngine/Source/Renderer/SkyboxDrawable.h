#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       SkyboxDrawable.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a drawable skybox component using a cubemap texture.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"

#include <array>
#include <string>
#include <memory>

#include "Renderer/IDrawable.h"

struct SkyboxDrawable : IDrawable
{
    std::array<std::string, 6> faces{};
    
    u32 cubemapTexture = 0;
    u32 vao = 0;
    u32 vbo = 0;

    SkyboxDrawable() = delete;
    explicit SkyboxDrawable(const std::array<std::string, 6>& faces_);
    ~SkyboxDrawable();

    void draw(Renderer& renderer, const Transform& transform) const override;

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::DrawableSkybox;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<SkyboxDrawable>(faces);
    }

private:
    void init_cube();
    void destroy();
};