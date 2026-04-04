#include "pch.h"
#include "EditorLayer.h"

#include "Core/AppContext.h"
#include "Core/Input.h"
#include "Core/Window.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"

#include <vector>

void EditorLayer::on_attach(AppContext& ctx)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(ctx.pWindow->window, true);
    ImGui_ImplOpenGL3_Init();

    glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void EditorLayer::on_detach(AppContext& ctx)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorLayer::on_update(AppContext& ctx, f32 deltaTime)
{
    Input& input = *ctx.pInput;
    Scene& scene = *ctx.pScene;

    if (input.is_key_pressed(GLFW_KEY_SPACE))
    {
        editMode = !editMode;
        input.mouseBegin = true;

        if (editMode)
            glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (input.is_key_pressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(ctx.pWindow->window, GLFW_TRUE);

    if (editMode)
        return;

    Camera& cam = scene.cameras[scene.currentCameraIdx];

    if (input.is_key_down(GLFW_KEY_W))
        cam.process_keyboard_forward(deltaTime);
    if (input.is_key_down(GLFW_KEY_S))
        cam.process_keyboard_backward(deltaTime);
    if (input.is_key_down(GLFW_KEY_A))
        cam.process_keyboard_left(deltaTime);
    if (input.is_key_down(GLFW_KEY_D))
        cam.process_keyboard_right(deltaTime);
    if (input.is_key_down(GLFW_KEY_Q))
        cam.process_keyboard_down(deltaTime);
    if (input.is_key_down(GLFW_KEY_E))
        cam.process_keyboard_up(deltaTime);

    cam.process_mouse_movement(input.mouseDeltaX, input.mouseDeltaY);
}

void EditorLayer::on_render(AppContext& ctx)
{
}

void EditorLayer::on_ui_render(AppContext& ctx)
{
    if (!editMode)
        return;

    Scene& scene = *ctx.pScene;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    if (showImGuiDemo)
        ImGui::ShowDemoWindow();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
            }

            if (ImGui::MenuItem("Open"))
                scene.load_from_file("Resources/Scenes/scene.ill");

            if (ImGui::MenuItem("Save"))
                scene.save_to_file("Resources/Scenes/scene.ill");

            if (ImGui::MenuItem("Exit"))
                glfwSetWindowShouldClose(ctx.pWindow->window, GLFW_TRUE);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editor"))
        {
            ImGui::MenuItem("Show Demo ImGui Window", nullptr, &showImGuiDemo);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Editor");

    if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::vector<const char*> names;
        for (const auto& obj : scene.objects)
            names.push_back(obj.name.c_str());

        if (!scene.objects.empty())
        {
            if (selectedObject >= (i32)scene.objects.size())
                selectedObject = (i32)scene.objects.size() - 1;

            ImGui::ListBox("Select object", &selectedObject, names.data(), (i32)names.size());

            auto& obj = scene.objects[selectedObject];
            ImGui::DragFloat3("Position", &obj.transform.position.x, 0.01f, -100.0f, 100.0f, "%.2f");
            ImGui::DragFloat3("Rotation", &obj.transform.rotation.x, 1.0f, -180.0f, 180.0f, "%.2f");
            ImGui::DragFloat3("Scale", &obj.transform.scale.x, 0.01f, 0.1f, 50.0f, "%.2f");
            ImGui::DragFloat3("Color", &obj.color.x, 0.01f, 0.0f, 1.0f, "%.2f");
        }
    }

    if (ImGui::CollapsingHeader("Cameras", ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::vector<const char*> names;
        for (const auto& cam : scene.cameras)
            names.push_back(cam.name.c_str());

        if (!scene.cameras.empty())
        {
            i32 current = (i32)scene.currentCameraIdx;
            ImGui::ListBox("Select camera", &current, names.data(), (i32)names.size(), 5);
            scene.currentCameraIdx = (u32)current;
        }

        if (ImGui::Button("Add camera"))
            scene.add_camera(Camera());

        if (ImGui::Button("Remove camera"))
            scene.remove_camera(scene.currentCameraIdx);
    }

    ImGui::End();

    ImGui::Begin("Info");
    ImGui::Text("FPS: %u", 0);
    ImGui::Text("Mouse position: (%f, %f)", ctx.pInput->mouseX, ctx.pInput->mouseY);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}