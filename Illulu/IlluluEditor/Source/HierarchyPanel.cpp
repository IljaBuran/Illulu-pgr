#include "pch.h"
#include "HierarchyPanel.h"

#include "EditorState.h"

#include "Core/AppContext.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"
#include "Scene/PerspectiveCameraComponent.h"
#include "Scene/IComponent.h"

#include <optional>

static std::optional<ComponentRef> find_camera_component_ref(Scene& scene, u32 objectIdx)
{
    const u32 size = static_cast<u32>(scene.cachedCameras.size());
    for (u32 i = 0; i < size; i++)
    {
        if (scene.cachedCameras[i].objectIdx == objectIdx)
            return scene.cachedCameras[i];
    }
    return std::nullopt;
}

void HierarchyPanel::clamp_selection(Scene& scene, EditorState& state)
{
    if (scene.objects.empty())
    {
        state.selectedObject = -1;
        return;
    }

    if (state.selectedObject < 0)
        state.selectedObject = 0;

    if (state.selectedObject >= static_cast<i32>(scene.objects.size()))
        state.selectedObject = static_cast<i32>(scene.objects.size()) - 1;
}

void HierarchyPanel::draw(AppContext& ctx, EditorState& state)
{
    Scene& scene = *ctx.pScene;

    ImGui::Begin("Hierarchy");

    if (scene.objects.empty())
    {
        ImGui::Text("No objects in scene.");

        if (ImGui::BeginPopupContextWindow("HierarchyBackgroundPopup", ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Add Empty Object"))
            {
                Object obj;
                obj.name = "Empty";
                scene.add_object(std::move(obj));
            }

            if (ImGui::MenuItem("Add Camera"))
            {
                Object cam;
                cam.name = "Camera";
                scene.add_object(std::move(cam));
                scene.add_object_component<PerspectiveCameraComponent>(static_cast<u32>(scene.objects.size() - 1));
            }

            ImGui::EndPopup();
        }

        ImGui::End();
        return;
    }

    clamp_selection(scene, state);

    if (ImGui::BeginListBox("##objects", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 12.0f)))
    {
        for (i32 i = 0; i < static_cast<i32>(scene.objects.size()); i++)
        {
            bool isSelected = (state.selectedObject == i);

            if (ImGui::Selectable(scene.objects[i].name.c_str(), isSelected))
                state.selectedObject = i;

            if (isSelected)
                ImGui::SetItemDefaultFocus();

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete"))
                {
                    const i32 objectToDelete = i;

                    if (scene.remove_object(objectToDelete))
                    {
                        if (scene.objects.empty())
                            state.selectedObject = -1;
                        else if (state.selectedObject >= static_cast<i32>(scene.objects.size()))
                            state.selectedObject = static_cast<i32>(scene.objects.size()) - 1;
                    }
                }

                if (ImGui::MenuItem("Duplicate"))
                {
                    scene.duplicate_object(i);
                }

                if (ImGui::MenuItem("Rename"))
                {
                    // todo(ilja): rename implement
                }

                if (auto camRef = find_camera_component_ref(scene, static_cast<u32>(i)); camRef.has_value())
                {
                    if (ImGui::MenuItem("Set as current camera"))
                        scene.currentCamera = *camRef;
                }

                ImGui::EndPopup();
            }
        }

        if (ImGui::BeginPopupContextWindow("HierarchyBackgroundPopup", ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Add Empty Object"))
            {
                Object obj;
                obj.name = "Empty";
                scene.add_object(std::move(obj));
                state.selectedObject = static_cast<i32>(scene.objects.size()) - 1;
            }

            if (ImGui::MenuItem("Add Camera"))
            {
                Object cam;
                cam.name = "Camera";
                scene.add_object(std::move(cam));
                scene.add_object_component<PerspectiveCameraComponent>(static_cast<u32>(scene.objects.size() - 1));
                state.selectedObject = static_cast<i32>(scene.objects.size()) - 1;
            }

            ImGui::EndPopup();
        }

        ImGui::EndListBox();
    }

    ImGui::End();
}