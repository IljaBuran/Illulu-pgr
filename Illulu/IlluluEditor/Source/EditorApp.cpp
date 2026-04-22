#include "pch.h"
#include "EditorApp.h"

#include <algorithm>

#include "EditorState.h"
#include "MenuBarPanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "InfoPanel.h"
#include "AssetPanel.h"

#include "Core/AppContext.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"
#include "Scene/PerspectiveCameraComponent.h"

#include "stb/stb_image.h"

EditorApp::EditorApp() = default;
EditorApp::~EditorApp() = default;

bool EditorApp::init()
{
    if (!engine.init())
        return false;

    AppContext& ctx = engine.get_context();

    state = std::make_unique<EditorState>();

    menuBarPanel = std::make_unique<MenuBarPanel>();
    hierarchyPanel = std::make_unique<HierarchyPanel>();
    inspectorPanel = std::make_unique<InspectorPanel>();
    infoPanel = std::make_unique<InfoPanel>();
    assetPanel = std::make_unique<AssetPanel>();

    NFD::Init();
    NFD_SetDisplayPropertiesFromGLFW();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

    static std::string imguiLayoutPath = "Resources/Editor/imgui_layout.ini";
    io.IniFilename = imguiLayoutPath.c_str();

    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;

    ImGui_ImplGlfw_InitForOpenGL(ctx.pWindow->window, true);
    ImGui_ImplOpenGL3_Init();

    glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    GLFWimage images[1];
    images[0].pixels = stbi_load("Resources/Icons/editor_icon.png", &images[0].width, &images[0].height, 0, 4);

    if (images[0].pixels)
    {
        glfwSetWindowIcon(ctx.pWindow->window, 1, images);
        stbi_image_free(images[0].pixels);
    }

    return true;
}

i32 EditorApp::run()
{
    while (!engine.should_close())
    {
        engine.begin_frame();

        const f32 deltaTime = engine.get_delta_time();

        update(deltaTime);

        engine.update();

        render();
        engine.render();

        ui_render();

        engine.end_frame();
    }

    shutdown();
    return 0;
}

void EditorApp::shutdown()
{
    assetPanel.reset();
    infoPanel.reset();
    inspectorPanel.reset();
    hierarchyPanel.reset();
    menuBarPanel.reset();
    state.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    NFD::Quit();

    engine.shutdown();
}

void EditorApp::update(f32 deltaTime)
{
    AppContext& ctx = engine.get_context();
    Input& input = *ctx.pInput;
    Scene& scene = *ctx.pScene;

    if (input.is_key_pressed(GLFW_KEY_SPACE) && !state->locked)
    {
        state->editMode = !state->editMode;
        input.mouseBegin = true;

        if (state->editMode)
            glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (state->editMode)
        return;

    if (scene.currentCamera.objectIdx >= scene.objects.size())
        return;

    Object& camObj = scene.objects[scene.currentCamera.objectIdx];

    if (scene.currentCamera.componentIdx >= camObj.components.size())
        return;

    PerspectiveCameraComponent* cam =
        reinterpret_cast<PerspectiveCameraComponent*>(camObj.components[scene.currentCamera.componentIdx].get());

    if (!cam)
        return;

    glm::vec3 front = cam->get_front();
    glm::vec3 right = glm::normalize(glm::cross(front, cam->worldUp));

    f32 movementSpeed = 9.0f;
    constexpr f32 mouseSensitivity = 0.1f;

    if (input.is_key_down(GLFW_KEY_LEFT_SHIFT))
        movementSpeed *= 2;

    if (input.is_key_down(GLFW_KEY_W))
        camObj.transform.position += front * movementSpeed * deltaTime;
    if (input.is_key_down(GLFW_KEY_S))
        camObj.transform.position -= front * movementSpeed * deltaTime;
    if (input.is_key_down(GLFW_KEY_A))
        camObj.transform.position -= right * movementSpeed * deltaTime;
    if (input.is_key_down(GLFW_KEY_D))
        camObj.transform.position += right * movementSpeed * deltaTime;
    if (input.is_key_down(GLFW_KEY_Q))
        camObj.transform.position -= cam->worldUp * movementSpeed * deltaTime;
    if (input.is_key_down(GLFW_KEY_E))
        camObj.transform.position += cam->worldUp * movementSpeed * deltaTime;

    cam->yaw += input.mouseDeltaX * mouseSensitivity;
    cam->pitch += input.mouseDeltaY * mouseSensitivity;
    cam->pitch = std::clamp(cam->pitch, -89.0f, 89.0f);
}

void EditorApp::render()
{
}

void EditorApp::begin_imgui_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EditorApp::end_imgui_frame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorApp::ui_render()
{
    if (!state || !state->editMode)
        return;

    AppContext& ctx = engine.get_context();

    begin_imgui_frame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    if (state->showImGuiDemo)
        ImGui::ShowDemoWindow();

    menuBarPanel->draw(ctx, *state);
    hierarchyPanel->draw(ctx, *state);
    inspectorPanel->draw(ctx, *state);
    infoPanel->draw(ctx, *state);
    assetPanel->draw(ctx, *state);

    end_imgui_frame();
}