#include "pch.h"
#include "EditorApp.h"

#include <algorithm>
#include <memory>

#include "EditorState.h"
#include "MenuBarPanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "InfoPanel.h"
#include "AssetPanel.h"
#include "Scripts.h"

#include "Core/AppContext.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/Debug.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"
#include "Scene/PerspectiveCameraComponent.h"
#include "Scene/PointLightComponent.h"
#include "Renderer/ModelDrawable.h"
#include "Renderer/Renderer.h"

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

    GLFWimage images[1]{};
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
    // note(ilja): this is such a mess...
    AppContext& ctx = engine.get_context();
    Input& input = *ctx.pInput;
    Scene& scene = *ctx.pScene;
    Renderer& renderer = *ctx.pRenderer;


    // note(ilja): picking
    if (input.is_key_pressed(GLFW_KEY_T) && state->editMode && !state->locked)
    {
        i32 pickedObjectIdx = renderer.pick_object(input.mouseX, input.mouseY);

        if (pickedObjectIdx >= 0)
        {
            state->selectedObject = pickedObjectIdx;
        }
    }

    // note(ilja): switch between editMode
    if (input.is_key_pressed(GLFW_KEY_E) && !state->locked)
    {
        const bool switchingToEditMode = !state->editMode;

        state->editMode = !state->editMode;
        input.mouseBegin = true;

        ImGuiIO& io = ImGui::GetIO();

        if (switchingToEditMode)
        {
            io.ClearEventsQueue();
            io.ClearInputKeys();
            io.ClearInputMouse();

            glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            io.ClearEventsQueue();
            io.ClearInputKeys();
            io.ClearInputMouse();

            glfwSetInputMode(ctx.pWindow->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    if (state->editMode)
        return;

    if (scene.currentCamera.objectIdx >= scene.objects.size())
        return;

    Object& camObj = scene.objects[scene.currentCamera.objectIdx];

    if (scene.currentCamera.componentIdx >= camObj.components.size())
        return;

    PerspectiveCameraComponent* cam =
        static_cast<PerspectiveCameraComponent*>(camObj.components[scene.currentCamera.componentIdx].get());

    if (!cam)
        return;

    glm::vec3 front = cam->get_front();
    glm::vec3 right = glm::normalize(glm::cross(front, cam->worldUp));

    f32 movementSpeed = 9.0f;
    const f32 mouseSensitivity = 0.1f;

    if (scene.currentCamera.objectIdx == 1 || input.is_key_down(GLFW_KEY_LEFT_CONTROL))
        movementSpeed /= 4;
    else if (input.is_key_down(GLFW_KEY_LEFT_SHIFT))
        movementSpeed *= 2;

    // note(ilja): store previous position in case we collide with world boundary
    glm::vec3 oldPos = camObj.transform.position;

    if (scene.currentCamera.objectIdx != 1)
    {
        if (input.is_key_down(GLFW_KEY_W))
            camObj.transform.position += front * movementSpeed * deltaTime;
        if (input.is_key_down(GLFW_KEY_S))
            camObj.transform.position -= front * movementSpeed * deltaTime;
        if (input.is_key_down(GLFW_KEY_A))
            camObj.transform.position -= right * movementSpeed * deltaTime;
        if (input.is_key_down(GLFW_KEY_D))
            camObj.transform.position += right * movementSpeed * deltaTime;
    }
    else // note(ilja): hardcode for camera in obj 1 to move like person would
    {
        if (input.is_key_down(GLFW_KEY_W))
            camObj.transform.position += glm::normalize(glm::vec3{front.x, 0, front.z}) * movementSpeed * deltaTime;
        if (input.is_key_down(GLFW_KEY_S))
            camObj.transform.position -= glm::normalize(glm::vec3{front.x, 0, front.z}) * movementSpeed * deltaTime;
        if (input.is_key_down(GLFW_KEY_A))
            camObj.transform.position -= glm::normalize(glm::vec3{right.x, 0, right.z}) * movementSpeed * deltaTime;
        if (input.is_key_down(GLFW_KEY_D))
            camObj.transform.position += glm::normalize(glm::vec3{right.x, 0, right.z}) * movementSpeed * deltaTime;
    }

    cam->yaw += input.mouseDeltaX * mouseSensitivity;
    cam->pitch += input.mouseDeltaY * mouseSensitivity;
    cam->pitch = std::clamp(cam->pitch, -89.0f, 89.0f);

    scene.update_collisions();
    

    static LerpWaterObject lerpWaterObj;
    static u32 animCnter = 0; // note(ilja): even -> is down, odd -> up

    // note(ilja): collision checking
    // note(ilja): hardcoded functionality -> it'll break if scene(object order, names, etc...) changes...
    bool waterSinkCollision = false;
    bool womanCollision = false;
    bool tvCollision = false;
    bool lightSwitchCollision = false;
    bool worldBoundaryCollision = false;
    for (const auto& collision : scene.currentCollisions)
    {
        if ((collision.first == "KitchenCollision" && collision.second == "Player") ||
            (collision.first == "Player" && collision.second == "KitchenCollision"))
        {
            waterSinkCollision = true;
            continue;
        }
        if ((collision.first == "Doggo" && collision.second == "Player") ||
            (collision.first == "Player" && collision.second == "Doggo"))
        {
            womanCollision = true;
            continue;
        }
        if ((collision.first == "animatedTex" && collision.second == "Player") ||
            (collision.first == "Player" && collision.second == "animatedTex"))
        {
            tvCollision = true;
            continue;
        }
        if ((collision.first == "LightSwitch" && collision.second == "Player") ||
            (collision.first == "Player" && collision.second == "LightSwitch"))
        {
            lightSwitchCollision = true;
            continue;
        }
        if (true)
        {
            if ((collision.first == "Wall1" && collision.second == "Player") ||
                (collision.first == "Player" && collision.second == "Wall1"))
            {
                worldBoundaryCollision = true;
                continue;
            }
            if ((collision.first == "Wall2" && collision.second == "Player") ||
                (collision.first == "Player" && collision.second == "Wall2"))
            {
                worldBoundaryCollision = true;
                continue;
            }
            if ((collision.first == "Wall3" && collision.second == "Player") ||
                (collision.first == "Player" && collision.second == "Wall3"))
            {
                worldBoundaryCollision = true;
                continue;
            }
            if ((collision.first == "Wall4" && collision.second == "Player") ||
                (collision.first == "Player" && collision.second == "Wall4"))
            {
                worldBoundaryCollision = true;
                continue;
            }
            if ((collision.first == "KitchenIsland" && collision.second == "Player") ||
                (collision.first == "Player" && collision.second == "KitchenIsland"))
            {
                worldBoundaryCollision = true;
                continue;
            }
            if ((collision.first == "BedCollision" && collision.second == "Player") ||
                (collision.first == "Player" && collision.second == "BedCollision"))
            {
                worldBoundaryCollision = true;
                continue;
            }
        }
    }
    
    // note(ilja): responsed to collisions
    if (input.is_key_pressed(GLFW_KEY_F) && !lerpWaterObj.playing && waterSinkCollision)
    {
        if (animCnter % 2 == 0)
        {
            lerpWaterObj.start(2.0f, static_cast<f32>(glfwGetTime()), 47u, {8.63f, -0.29f, 2.96f}, {8.63f, -0.21f, 2.96f});
        }
        else
        {
            lerpWaterObj.start(2.0f, static_cast<f32>(glfwGetTime()), 47u, {8.63f, -0.21f, 2.96f}, {8.63f, -0.29f, 2.96f});
        }
        animCnter++;
    }
    if (input.is_key_pressed(GLFW_KEY_F) && womanCollision)
    {
        message_box("Doggo", "Baf!").pop_up();
    }
    if (input.is_key_pressed(GLFW_KEY_F) && tvCollision)
    {
        static bool& vis = scene.objects[37].get_component<ModelDrawable>()->visible;
        vis = !vis;
    }
    if (input.is_key_pressed(GLFW_KEY_F) && lightSwitchCollision)
    {
        static f32& lightIntensity = static_cast<PointLightComponent*>(scene.objects[32].components[0].get())->intensity;
        
        lightIntensity = lightIntensity == 0.0f ? 2.05f : 0.0f;
    }
    if (worldBoundaryCollision)
    {
        camObj.transform.position = oldPos;
    }


    lerpWaterObj.update(static_cast<f32>(glfwGetTime()), scene);
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