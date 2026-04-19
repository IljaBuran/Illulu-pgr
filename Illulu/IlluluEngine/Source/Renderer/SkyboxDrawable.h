#pragma once

#include <array>
#include <string>
#include <memory>

#include "Renderer/IDrawable.h"
#include "Common.h"

struct SkyboxDrawable : IDrawable
{
    std::array<std::string, 6> faces{};
    
    u32 cubemapTexture = 0;
    u32 vao = 0;
    u32 vbo = 0;

    SkyboxDrawable() = delete;
    explicit SkyboxDrawable(const std::array<std::string, 6>& faces_);
    ~SkyboxDrawable();

    SkyboxDrawable(const SkyboxDrawable&) = delete;
    SkyboxDrawable& operator=(const SkyboxDrawable&) = delete;

    SkyboxDrawable(SkyboxDrawable&& other) noexcept;
    SkyboxDrawable& operator=(SkyboxDrawable&& other) noexcept;

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