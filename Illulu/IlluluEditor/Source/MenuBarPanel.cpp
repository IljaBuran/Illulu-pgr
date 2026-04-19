#include "pch.h"
#include "MenuBarPanel.h"

#include "Scene/Scene.h"
#include "Core/Window.h"
#include "Core/Debug.h"

#include <iostream>


void MenuBarPanel::draw(AppContext& ctx, EditorState& state)
{
    Scene& scene = *ctx.pScene;

    using MSG_TYPE = message_box::MSG_BOX_TYPE;

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
                if (message_box("Exit", "Do you really wish to exit? (destructive)", message_box::MSG_BOX_TYPE::OK_NO).pop_up())
                    glfwSetWindowShouldClose(ctx.pWindow->window, GLFW_TRUE);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editor"))
        {
            ImGui::MenuItem("Show Demo ImGui Window", nullptr, &state.showImGuiDemo);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}