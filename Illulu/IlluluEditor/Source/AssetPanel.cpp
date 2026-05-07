//-----------------------------------------------------------------------------------------
/**
 * \file       AssetPanel.cpp
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Implements the asset panel for importing, listing, selecting, removing, and spawning model assets.
 */
 //-----------------------------------------------------------------------------------------

#include "pch.h"
#include "AssetPanel.h"

#include "EditorState.h"

#include "Core/AppContext.h"
#include "Core/Debug.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"
#include "Renderer/Model.h"
#include "Renderer/ModelDrawable.h"

#include <filesystem>

void AssetPanel::clamp_selection(Scene& scene, EditorState& state)
{
    if (scene.models.empty())
    {
        state.selectedModel = -1;
        return;
    }

    if (state.selectedModel < 0)
        state.selectedModel = 0;

    if (state.selectedModel >= static_cast<i32>(scene.models.size()))
        state.selectedModel = static_cast<i32>(scene.models.size()) - 1;
}

void AssetPanel::import_model(Scene& scene, EditorState& state)
{
    NFD::UniquePath path;

    nfdfilteritem_t filterItems[] =
    {
        { "3D Models", "obj,fbx,gltf,glb,dae,blend" }
    };

    nfdresult_t result = NFD::OpenDialog(path, filterItems, 1);

    if (result == NFD_OKAY)
    {
        auto lMakeResourceRelativePath = [](const std::string& absolutePath) -> std::string
        {
            namespace fs = std::filesystem;

            fs::path abs = fs::absolute(absolutePath);
            fs::path current = fs::current_path();
            fs::path relative = fs::relative(abs, current);

            return relative.generic_string();
        };

        std::string relativePath = lMakeResourceRelativePath(path.get());
        
        Model model = ModelLoader(relativePath.c_str());
        scene.add_model(std::move(model));

        state.selectedModel = static_cast<i32>(scene.models.size()) - 1;
    }
    else if (result == NFD_ERROR)
    {
        message_box("File Dialog Error", "Unexpected NFD_ERROR while opening model file dialog").pop_up();
    }
}

void AssetPanel::spawn_model_object(Scene& scene, u32 modelIdx)
{
    if (modelIdx >= scene.models.size())
        return;

    Object obj;
    obj.name = scene.models[modelIdx].name;
    scene.add_object(std::move(obj));

    const u32 objectIdx = static_cast<u32>(scene.objects.size() - 1);
    scene.add_object_component<ModelDrawable>(objectIdx, modelIdx, Color(1.0f, 1.0f, 1.0f));
}

void AssetPanel::draw(AppContext& ctx, EditorState& state)
{
    Scene& scene = *ctx.pScene;

    ImGui::Begin("Assets");

    if (ImGui::Button("Import Model", ImVec2(-FLT_MIN, 0.0f)))
    {
        import_model(scene, state);
    }

    ImGui::Separator();

    if (scene.models.empty())
    {
        ImGui::Text("No models imported.");
        ImGui::TextDisabled("Use 'Import Model' to add OBJ/FBX/GLTF assets.");
        ImGui::End();
        return;
    }

    clamp_selection(scene, state);

    if (ImGui::BeginListBox("##models", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 12.0f)))
    {
        for (i32 i = 0; i < static_cast<i32>(scene.models.size()); i++)
        {
            const Model& model = scene.models[i];
            const bool isSelected = (state.selectedModel == i);

            if (ImGui::Selectable(model.name.c_str(), isSelected))
                state.selectedModel = i;

            if (isSelected)
                ImGui::SetItemDefaultFocus();

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                spawn_model_object(scene, static_cast<u32>(i));
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Create object in scene"))
                {
                    spawn_model_object(scene, static_cast<u32>(i));
                }

                if (ImGui::MenuItem("Remove asset"))
                {
                    scene.remove_model(static_cast<u32>(i));

                    if (scene.models.empty())
                        state.selectedModel = -1;
                    else if (state.selectedModel >= static_cast<i32>(scene.models.size()))
                       state.selectedModel = static_cast<i32>(scene.models.size()) - 1;

                    ImGui::EndPopup();
                    break;
                }

                ImGui::EndPopup();
            }
        }

        ImGui::EndListBox();
    }

    ImGui::Separator();

    if (state.selectedModel >= 0 && state.selectedModel < static_cast<i32>(scene.models.size()))
    {
        const Model& model = scene.models[state.selectedModel];

        ImGui::Text("Selected Asset");
        ImGui::Separator();

        ImGui::Text("Name:");
        ImGui::TextWrapped("%s", model.name.c_str());

        ImGui::Spacing();

        ImGui::Text("Source Path:");
        ImGui::TextWrapped("%s", model.sourcePath.c_str());

        ImGui::Spacing();

        ImGui::Text("Meshes: %d", static_cast<int>(model.meshes.size()));

        if (ImGui::Button("Create Object", ImVec2(-FLT_MIN, 0.0f)))
        {
            spawn_model_object(scene, static_cast<u32>(state.selectedModel));
        }
    }

    ImGui::End();
}