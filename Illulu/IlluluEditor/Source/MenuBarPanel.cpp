//-----------------------------------------------------------------------------------------
/**
 * \file       MenuBarPanel.cpp
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Implements the editor menu bar with file, view, custom function, and help actions.
 */
 //-----------------------------------------------------------------------------------------

#include "pch.h"
#include "MenuBarPanel.h"

#include "Scene/Scene.h"
#include "Core/Window.h"
#include "Core/Debug.h"

#include "Renderer/Renderer.h"
#include <glm/glm.hpp>
#include <iostream>


void MenuBarPanel::draw(AppContext& ctx, EditorState& state)
{
    Scene& scene = *ctx.pScene;

    using MSG_TYPE = message_box::MSG_BOX_TYPE;

    static bool openRotateAllPopup = false;
    static glm::vec3 rotateAnglesDeg{0.0f, 0.0f, 0.0f};

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                message_box("DEBUG_EDITOR", "Unimplemented yet - only resets the selected", MSG_TYPE::DEFAULT).pop_up();
                state.selectedObject = -1;
            }

            if (ImGui::MenuItem("Save"))
            {
                NFD::UniquePath outPath;
                nfdfilteritem_t filterItem[1] = {{"Illulu Scenes", "ill"}};
                nfdresult_t dialogResult = NFD::SaveDialog(outPath, filterItem, 1);

                if (dialogResult == NFD_OKAY)
                {
                    scene.save_to_file(outPath.get());
                }
                else if (dialogResult == NFD_ERROR)
                {
                    FATAL_CRASH("Unexpected NFD_ERROR");
                }
            }

            if (ImGui::MenuItem("Load scene(destructive)"))
            {
                NFD::UniquePath path;
                nfdfilteritem_t filterItem[1] = {{"Illulu Scenes", "ill"}};
                nfdresult_t dialogResult = NFD::OpenDialog(path, filterItem, 1);

                if (dialogResult == NFD_OKAY)
                {
                    scene.load_from_file(path.get());
                }
                else if (dialogResult == NFD_ERROR)
                {
                    FATAL_CRASH("Unexpected NFD_ERROR");
                }
            }

            if (ImGui::MenuItem("Exit"))
            {
                if (message_box("Exit", "Do you really wish to exit? (destructive)", MSG_TYPE::OK_NO).pop_up())
                    glfwSetWindowShouldClose(ctx.pWindow->window, GLFW_TRUE);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            static bool collisionBoxes = false;
            static bool normals = false;

            if (ImGui::MenuItem("Show Collision Boxes", nullptr, collisionBoxes))
            {
                ctx.pRenderer->toggle_collision_boxes_view();
                collisionBoxes = !collisionBoxes;
            }

            if (ImGui::MenuItem("Normal view", nullptr, normals))
            {
                ctx.pRenderer->toggle_normal_view();
                normals = !normals;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Custom Functions"))
        {
            if (ImGui::MenuItem("Rotate all objects around origin"))
            {
                openRotateAllPopup = true;
            }

            if (ImGui::MenuItem("Recompile all shaders (currently basic, skybox, debugline)"))
            {
                ctx.pRenderer->reload_shaders();
            }

            bool enableCulling = ctx.pRenderer->enableCulling;
            if (ImGui::MenuItem("Enable Culling", nullptr, enableCulling))
            {
                ctx.pRenderer->toggle_culling();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Show Demo ImGui Window", nullptr, &state.showImGuiDemo);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (openRotateAllPopup)
    {
        ImGui::OpenPopup("Rotate all objects around origin");
        openRotateAllPopup = false;
    }

    if (ImGui::BeginPopupModal("Rotate all objects around origin", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Set rotation delta in degrees.");
        ImGui::Separator();

        ImGui::DragFloat3(
            "Rotation",
            &rotateAnglesDeg.x,
            1.0f,
            -360.0f,
            360.0f,
            "%.1f deg"
        );

        if (ImGui::Button("Apply", ImVec2(120, 0)))
        {
            scene.rotate_all_objects_around_origin(rotateAnglesDeg);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
