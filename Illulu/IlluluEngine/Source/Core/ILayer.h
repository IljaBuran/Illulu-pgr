#pragma once

#include "Common.h"

struct AppContext;

struct ILayer
{
	virtual ~ILayer() = default;

	virtual void on_attach(AppContext& appCtx) = 0;
	virtual void on_detach(AppContext& appCtx) = 0;

	virtual void on_update(AppContext& appCtx, f32 deltaTime) = 0;
	virtual void on_render(AppContext& appCtx) = 0;
	virtual void on_ui_render(AppContext& appCtx) = 0;
};

