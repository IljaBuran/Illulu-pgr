//-----------------------------------------------------------------------------------------
/**
 * \file       InspectorPanel.cpp
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Implements the inspector panel for editing selected object transforms and components.
 */
 //-----------------------------------------------------------------------------------------

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
#include "Scene/DirectionalLightComponent.h"
#include "Scene/PointLightComponent.h"
#include "Scene/SpotLightComponent.h"
#include "Scene/OrthographicCameraComponent.h"
#include "Scene/CollisionBoxComponent.h"
#include "Scene/DistanceFogComponent.h"
#include "Scene/OrbitingMovementComponent.h"
#include "Scene/AnimatedTextureComponent.h"
#include "Scene/FloatingTextureComponent.h"

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
                ImGui::Checkbox("Visible", &cast->visible);
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
                // note(ilja): no usage for now
                [[maybe_unused]] SkyboxDrawable* cast = static_cast<SkyboxDrawable*>(comp.get());
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
        else if (kind == ComponentKind::DistanceFog)
        {
            const bool opened = ImGui::TreeNode("Distance Fog");

            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;

                ImGui::EndPopup();
            }

            if (opened)
            {
                DistanceFogComponent* cast = static_cast<DistanceFogComponent*>(comp.get());

                const char* fogModes[] =
                {
                    "Off",
                    "Linear",
                    "Exponential"
                };

                int mode = static_cast<int>(cast->mode);

                if (ImGui::Combo("Mode", &mode, fogModes, IM_ARRAYSIZE(fogModes)))
                {
                    cast->mode = static_cast<DistanceFogType>(mode);
                }

                ImGui::ColorEdit3("Fog Color", &cast->color.x);

                ImGui::Separator();

                if (cast->mode == DistanceFogType::Linear)
                {
                    ImGui::Text("Linear fog:");
                    ImGui::DragFloat("Start", &cast->start, 0.1f, 0.0f, 10000.0f, "%.2f");
                    ImGui::DragFloat("End", &cast->end, 0.1f, 0.01f, 10000.0f, "%.2f");

                    if (cast->end <= cast->start)
                        cast->end = cast->start + 0.1f;
                }
                else if (cast->mode == DistanceFogType::Exp)
                {
                    ImGui::Text("Exponential fog:");
                    ImGui::DragFloat("Density", &cast->density, 0.001f, 0.0f, 5.0f, "%.4f");
                }
                else
                {
                    ImGui::TextDisabled("Fog is disabled.");
                }

                ImGui::Separator();
                ImGui::TextWrapped(
                    "Distance fog is applied globally from the active camera, so attach to camera"
                );

                ImGui::TreePop();
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
        else if (kind == ComponentKind::CollisionBox)
        {
            const bool opened = ImGui::TreeNode("Collision Box");

            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;

                ImGui::EndPopup();
            }

            if (opened)
            {
                CollisionBoxComponent* cast = static_cast<CollisionBoxComponent*>(comp.get());

                ImGui::DragFloat3("Offset", &cast->offset.x, 0.01f, -100.0f, 100.0f, "%.2f");
                ImGui::DragFloat3("Size", &cast->size.x, 0.01f, 0.01f, 100.0f, "%.2f");

                if (cast->size.x < 0.01f) cast->size.x = 0.01f;
                if (cast->size.y < 0.01f) cast->size.y = 0.01f;
                if (cast->size.z < 0.01f) cast->size.z = 0.01f;

                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::OrbitingMovement)
        {
            const bool opened = ImGui::TreeNode("Orbiting Movement");

            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;

                ImGui::EndPopup();
            }

            if (opened)
            {
                OrbitingMovementComponent* cast = static_cast<OrbitingMovementComponent*>(comp.get());

                ImGui::DragFloat3("Center", &cast->center.x, 0.01f, -100.0f, 100.0f, "%.2f");

                const char* axes[] =
                {
                    "X axis",
                    "Y axis",
                    "Z axis"
                };

                int axis = static_cast<int>(cast->orbitingAxis);

                if (ImGui::Combo("Orbit Axis", &axis, axes, IM_ARRAYSIZE(axes)))
                {
                    cast->orbitingAxis = static_cast<OrbitingAxis>(axis);
                }

                ImGui::DragFloat("Radius", &cast->radius, 0.05f, 0.0f, 1000.0f, "%.2f");
                ImGui::DragFloat("Height", &cast->height, 0.05f, -1000.0f, 1000.0f, "%.2f");

                ImGui::DragFloat("Speed", &cast->speed, 0.01f, -20.0f, 20.0f, "%.3f");
                ImGui::DragFloat("Angle", &cast->angle, 0.01f, -10000.0f, 10000.0f, "%.3f");

                ImGui::Checkbox("Look At Center", &cast->lookAtCenter);

                if (cast->radius < 0.0f)
                    cast->radius = 0.0f;


                ImGui::TreePop();
            }
        }
        else if (kind == ComponentKind::AnimatedTexture)
        {
            const bool opened = ImGui::TreeNode("Animated Texture");

            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;

                ImGui::EndPopup();
            }

            if (opened)
            {
                AnimatedTextureComponent* cast = static_cast<AnimatedTextureComponent*>(comp.get());

                ImGui::DragInt("Columns", (i32*)&cast->columns, 1.0f, 1, 256);
                ImGui::DragInt("Rows", (i32*)&cast->rows, 1.0f, 1, 256);
                ImGui::DragInt("Frame Count", (i32*)&cast->frameCount, 1.0f, 1, 65536);

                const u32 maxFrames = cast->columns * cast->rows;

                if (cast->frameCount > maxFrames)
                    cast->frameCount = maxFrames;

                if (cast->frameCount < 1)
                    cast->frameCount = 1;

                ImGui::DragInt("Current Frame", (i32*)&cast->currentFrame, 1.0f, 0, cast->frameCount - 1);

                if (cast->currentFrame < 0)
                    cast->currentFrame = 0;

                if (cast->currentFrame >= cast->frameCount)
                    cast->currentFrame = cast->frameCount - 1;

                ImGui::DragFloat("Frame Time", &cast->frameTime, 0.001f, 0.001f, 10.0f, "%.4f");

                if (cast->frameTime < 0.001f)
                    cast->frameTime = 0.001f;

                ImGui::DragFloat("Timer", &cast->timer, 0.001f, 0.0f, 10.0f, "%.4f");

                if (cast->timer < 0.0f)
                    cast->timer = 0.0f;

                ImGui::Checkbox("Playing", &cast->playing);
                ImGui::Checkbox("Loop", &cast->loop);

                ImGui::Separator();

                ImGui::Text("Atlas: %d x %d", cast->columns, cast->rows);
                ImGui::Text("Max frames in atlas: %d", maxFrames);
                ImGui::Text("Current frame: %d / %d", cast->currentFrame, cast->frameCount - 1);

                ImGui::TreePop();
            }
        }
        
        else if (kind == ComponentKind::FloatingTexture)
        {
            const bool opened = ImGui::TreeNode("Floating Texture");

            if (ImGui::BeginPopupContextItem("component_ctx"))
            {
                if (ImGui::MenuItem("Delete component"))
                    deleteCompIdx = compIdx;

                ImGui::EndPopup();
            }

            if (opened)
            {
                FloatingTextureComponent* cast = static_cast<FloatingTextureComponent*>(comp.get());

                ImGui::DragFloat2("Speed", &cast->speed.x, 0.001f, -10.0f, 10.0f, "%.4f");
                ImGui::DragFloat2("Offset", &cast->offset.x, 0.001f, -100.0f, 100.0f, "%.4f");

                ImGui::Checkbox("Playing", &cast->playing);
                ImGui::Checkbox("Loop", &cast->loop);

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
            "DistanceFog",
            "DirectionalLight",
            "PointLight",
            "SpotLight",
            "CollisionBox",
            "OrbitingMovement",
            "AnimatedTexture",
            "FloatingTexture"
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
            else if (selectedKind == ComponentKind::DistanceFog)
            {
                scene.add_object_component<DistanceFogComponent>(state.selectedObject);
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
            else if (selectedKind == ComponentKind::CollisionBox)
            {
                scene.add_object_component<CollisionBoxComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::OrbitingMovement)
            {
                scene.add_object_component<OrbitingMovementComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::AnimatedTexture)
            {
                scene.add_object_component<AnimatedTextureComponent>(state.selectedObject);
            }
            else if (selectedKind == ComponentKind::FloatingTexture)
            {
                scene.add_object_component<FloatingTextureComponent>(state.selectedObject);
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