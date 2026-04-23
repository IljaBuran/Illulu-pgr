#include "pch.h"
#include "InspectorPanel.h"

#include "EditorState.h"

#include "Core/AppContext.h"
#include "Core/Debug.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"
#include "Scene/IComponent.h"
#include "Renderer/ModelDrawable.h"
#include "Renderer/SkyboxDrawable.h"
#include "Scene/PerspectiveCameraComponent.h"
#include "Scene/FreeCameraControllerComponent.h"
#include "Scene/DirectionalLightComponent.h"
#include "Scene/PointLightComponent.h"
#include "Scene/SpotLightComponent.h"
#include "Scene/OrthographicCameraComponent.h"

void InspectorPanel::draw(AppContext& ctx, EditorState& state)
{
    Scene& scene = *ctx.pScene;

    ImGui::Begin("Inspector");

    if (scene.objects.empty() ||
        state.selectedObject < 0 ||
        state.selectedObject >= static_cast<i32>(scene.objects.size()))
    {
        ImGui::Text("No object selected.");
        ImGui::End();
        return;
    }

    Object& obj = scene.objects[state.selectedObject];

    ImGui::Text("Name: %s", obj.name.c_str());
    ImGui::Separator();

    ImGui::DragFloat3("Position", &obj.transform.position.x, 0.01f, -100.0f, 100.0f, "%.2f");
    ImGui::DragFloat3("Rotation", &obj.transform.rotation.x, 1.0f, -180.0f, 180.0f, "%.2f");
    ImGui::DragFloat3("Scale", &obj.transform.scale.x, 0.01f, 0.1f, 50.0f, "%.2f");

    ImGui::Separator();
    ImGui::Text("Components: %d", static_cast<int>(obj.components.size()));

    int deleteCompIdx = -1;

    for (int compIdx = 0; compIdx < static_cast<int>(obj.components.size()); compIdx++)
    {
        auto& comp = obj.components[compIdx];

        ComponentKind kind = comp->get_component_kind();
        if (kind >= ComponentKind::COUNT)
        {
            message_box("InspectorPanel", "Invalid ComponentKind").pop_up();
            continue;
        }

        ImGui::PushID(compIdx);

        if (kind == ComponentKind::DrawableModel)
        {
            const bool opened = ImGui::TreeNode("Model");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;
                ImGui::EndPopup();
            }

            if (opened)
            {
                ModelDrawable* cast = static_cast<ModelDrawable*>(comp.get());

                if (scene.models.empty())
                {
                    ImGui::TextDisabled("No imported models available.");
                }
                else
                {
                    if (cast->modelIdx >= scene.models.size())
                        cast->modelIdx = 0;

                    const char* preview = scene.models[cast->modelIdx].name.c_str();

                    if (ImGui::BeginCombo("Asset", preview))
                    {
                        for (u32 modelIdx = 0; modelIdx < scene.models.size(); modelIdx++)
                        {
                            const bool isSelected = (cast->modelIdx == modelIdx);

                            if (ImGui::Selectable(scene.models[modelIdx].name.c_str(), isSelected))
                            {
                                cast->modelIdx = modelIdx;
                            }

                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }

                        ImGui::EndCombo();
                    }

                    ImGui::Text("Model Index: %u", cast->modelIdx);
                    ImGui::TextWrapped("Path: %s", scene.models[cast->modelIdx].sourcePath.c_str());
                }

                ImGui::Separator();
                ImGui::ColorEdit3("Tint", &cast->color.x);

                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::DrawableSkybox)
        {
            const bool opened = ImGui::TreeNode("Skybox");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;
                ImGui::EndPopup();
            }

            if (opened)
            {
                SkyboxDrawable* cast = static_cast<SkyboxDrawable*>(comp.get());
                (void)cast;
                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::PerspectiveCamera)
        {
            const bool opened = ImGui::TreeNode("Perspective Camera");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                bool enable = ctx.pScene->currentCamera != ComponentRef{static_cast<u32>(state.selectedObject), static_cast<u32>(compIdx)};
                if (ImGui::MenuItem("Delete component", 0, false, enable))
                    deleteCompIdx = compIdx;
                if (!enable)
                    ImGui::Text("Cannot delete current camera component");
                ImGui::EndPopup();
            }
            if (opened)
            {
                PerspectiveCameraComponent* cast = static_cast<PerspectiveCameraComponent*>(comp.get());

                ImGui::Separator();
                ImGui::DragFloat("Yaw", &cast->yaw, 0.5f, -360.0f, 360.0f, "%.2f");
                ImGui::DragFloat("Pitch", &cast->pitch, 0.5f, -89.0f, 89.0f, "%.2f");
                ImGui::DragFloat3("World Up", &cast->worldUp.x, 0.01f, -1.0f, 1.0f, "%.2f");

                ImGui::Separator();
                ImGui::DragFloat("FOV", &cast->fovDeg, 0.1f, 1.0f, 179.0f, "%.2f");
                ImGui::DragFloat("Near Plane", &cast->nearPlane, 0.01f, 0.001f, 1000.0f, "%.3f");
                ImGui::DragFloat("Far Plane", &cast->farPlane, 0.1f, 0.01f, 10000.0f, "%.2f");

                if (cast->farPlane <= cast->nearPlane)
                    cast->farPlane = cast->nearPlane + 0.1f;

                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::OrthographicCamera)
        {
            const bool opened = ImGui::TreeNode("Orthographic Camera");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                bool enable = ctx.pScene->currentCamera != ComponentRef{static_cast<u32>(state.selectedObject), static_cast<u32>(compIdx)};
                if (ImGui::MenuItem("Delete component", 0, false, enable))
                    deleteCompIdx = compIdx;
                if (!enable)
                    ImGui::Text("Cannot delete current camera component");
                ImGui::EndPopup();
            }
            
            if (opened)
            {
                OrthographicCameraComponent* cast = static_cast<OrthographicCameraComponent*>(comp.get());

                ImGui::Separator();
                ImGui::DragFloat("Yaw", &cast->yaw, 0.5f, -360.0f, 360.0f, "%.2f");
                ImGui::DragFloat("Pitch", &cast->pitch, 0.5f, -89.0f, 89.0f, "%.2f");
                ImGui::DragFloat3("World Up", &cast->worldUp.x, 0.01f, -1.0f, 1.0f, "%.2f");

                ImGui::Separator();
                ImGui::DragFloat("Ortho Height", &cast->orthoHeight, 0.1f, 0.1f, 1000.0f, "%.2f");
                ImGui::DragFloat("Near Plane", &cast->nearPlane, 0.1f, -10000.0f, 10000.0f, "%.2f");
                ImGui::DragFloat("Far Plane", &cast->farPlane, 0.1f, -10000.0f, 10000.0f, "%.2f");

                if (cast->farPlane <= cast->nearPlane)
                    cast->farPlane = cast->nearPlane + 0.1f;

                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::FreeCameraController)
        {
            ImGui::Text("no yet implemented");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;
                ImGui::EndPopup();
            }
        }
        else if (kind == ComponentKind::DirectionalLight)
        {
            const bool opened = ImGui::TreeNode("Directional Light");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;
                ImGui::EndPopup();
            }

            if (opened)
            {
                DirectionalLightComponent* cast = static_cast<DirectionalLightComponent*>(comp.get());
                ImGui::Text("Direction is driven by object rotation.");
                ImGui::ColorEdit3("Color", &cast->color.x);
                ImGui::DragFloat("Intensity", &cast->intensity, 0.05f, 0.0f, 20.0f, "%.2f");
                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::PointLight)
        {
            const bool opened = ImGui::TreeNode("Point Light");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;
                ImGui::EndPopup();
            }

            if (opened)
            {
                PointLightComponent* cast = static_cast<PointLightComponent*>(comp.get());
                ImGui::ColorEdit3("Color", &cast->color.x);
                ImGui::DragFloat("Intensity", &cast->intensity, 0.05f, 0.0f, 50.0f, "%.2f");
                ImGui::DragFloat("Constant", &cast->constant, 0.01f, 0.0f, 10.0f, "%.3f");
                ImGui::DragFloat("Linear", &cast->linear, 0.001f, 0.0f, 5.0f, "%.3f");
                ImGui::DragFloat("Quadratic", &cast->quadratic, 0.001f, 0.0f, 5.0f, "%.3f");
                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::SpotLight)
        {
            const bool opened = ImGui::TreeNode("Spot Light");
            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;
                ImGui::EndPopup();
            }

            if (opened)
            {
                SpotLightComponent* cast = static_cast<SpotLightComponent*>(comp.get());
                ImGui::Text("Direction is driven by object rotation.");
                ImGui::ColorEdit3("Color", &cast->color.x);
                ImGui::DragFloat("Intensity", &cast->intensity, 0.05f, 0.0f, 50.0f, "%.2f");
                ImGui::DragFloat("Inner Cutoff", &cast->innerCutoffDeg, 0.1f, 0.0f, 89.0f, "%.2f");
                ImGui::DragFloat("Outer Cutoff", &cast->outerCutoffDeg, 0.1f, 0.0f, 89.0f, "%.2f");
                ImGui::DragFloat("Constant", &cast->constant, 0.01f, 0.0f, 10.0f, "%.3f");
                ImGui::DragFloat("Linear", &cast->linear, 0.001f, 0.0f, 5.0f, "%.3f");
                ImGui::DragFloat("Quadratic", &cast->quadratic, 0.001f, 0.0f, 5.0f, "%.3f");

                if (cast->outerCutoffDeg < cast->innerCutoffDeg)
                    cast->outerCutoffDeg = cast->innerCutoffDeg;

                ImGui::TreePop();
            }
        }

        ImGui::PopID();
    }

    if (deleteCompIdx >= 0 && deleteCompIdx < static_cast<int>(obj.components.size()))
    {
        ctx.pScene->delete_object_component(state.selectedObject, deleteCompIdx);
    }

    if (ImGui::Button("Add new component", ImVec2(-FLT_MIN, 0.0f)))
    {
        ImGui::OpenPopup("add_new_comp_stack", 0);
    }

    if (ImGui::BeginPopupModal("add_new_comp_stack", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        state.locked = true;

        static int selectedItem = 0;
        static const char* items[] =
        {
            "DrawableModel",
            "DrawableSkybox",
            "PerspectiveCamera",
            "OrthographicCamera",
            "FreeCameraController",
            "DirectionalLight",
            "PointLight",
            "SpotLight"
        };

        ImGui::Combo("Component", &selectedItem, items, IM_ARRAYSIZE(items));

        if (ImGui::Button("Add"))
        {
            ComponentKind selectedKind = static_cast<ComponentKind>(selectedItem);

            if (selectedKind == ComponentKind::DrawableModel)
            {
                scene.add_object_component<ModelDrawable>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::DrawableSkybox)
            {
                message_box("Editor", "Not implemented yet").pop_up();
            }
            else if (selectedKind == ComponentKind::PerspectiveCamera)
            {
                scene.add_object_component<PerspectiveCameraComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::OrthographicCamera)
            {
                scene.add_object_component<OrthographicCameraComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::FreeCameraController)
            {
                scene.add_object_component<FreeCameraControllerComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::DirectionalLight)
            {
                scene.add_object_component<DirectionalLightComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::PointLight)
            {
                scene.add_object_component<PointLightComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::SpotLight)
            {
                scene.add_object_component<SpotLightComponent>(state.selectedObject);
            }

            ImGui::CloseCurrentPopup();
            state.locked = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
            state.locked = false;
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}