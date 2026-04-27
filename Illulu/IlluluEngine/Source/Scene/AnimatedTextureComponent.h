#pragma once

#include "Scene/IComponent.h"
#include "Common.h"

struct AnimatedTextureComponent : IComponent
{
    u32 columns = 1;
    u32 rows = 1;
    u32 frameCount = 1;

    f32 frameTime = 0.1f;

    u32 currentFrame = 0;
    f32 timer = 0.0f;

    bool playing = true;
    bool loop = true;

    AnimatedTextureComponent(u32 columns_ = 1, u32 rows_ = 1, u32 frameCount_ = 1, f32 frameTime_ = 0.1f,
        bool playing_ = true, bool loop_ = true)
        : columns(columns_), rows(rows_), frameCount(frameCount_), frameTime(frameTime_), playing(playing_), 
          loop(loop_) {}

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::AnimatedTexture;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<AnimatedTextureComponent>(*this);
    }
};