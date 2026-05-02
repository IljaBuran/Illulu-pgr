#include "pch.h"
#include "InfoPanel.h"

#include "EditorState.h"

#include "Core/AppContext.h"
#include "Core/Input.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"

void InfoPanel::draw(AppContext& ctx, EditorState& state)
{
    (void)state;

    ImGui::Begin("Info");

    ImGui::Text("FPS: %u", *ctx.lastFrameTime / 1000.0);
    ImGui::Text("Mouse position: (%f, %f)", ctx.pInput->mouseX, ctx.pInput->mouseY);

    Scene& scene = *ctx.pScene;
    ImGui::Text("Object count: %d", static_cast<int>(scene.objects.size()));
    ImGui::Text("Selected object ID: %u", state.selectedObject);

    if (scene.currentCamera.objectIdx < scene.objects.size())
    {
        const Object& camObj = scene.objects[scene.currentCamera.objectIdx];
        ImGui::Text("Current camera object: %s", camObj.name.c_str());
    }
    else
    {
        ImGui::Text("Current camera object: <invalid>");
    }

    ImGui::End();
}