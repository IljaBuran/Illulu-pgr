#pragma once
#include "IEditorPanel.h"

struct AssetPanel : public IEditorPanel
{
	const char* get_name() const override
	{
		return "Assets";
	}

	void draw(AppContext& ctx, EditorState& state) override;
private:

	void clamp_selection(struct Scene& scene, EditorState& state);
	void import_model(Scene& scene, EditorState& state);
	void spawn_model_object(struct Scene& scene, u32 modelIdx);
};

