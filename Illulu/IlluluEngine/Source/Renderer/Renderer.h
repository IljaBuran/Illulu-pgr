#pragma once

#include "Common.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "Renderer/Shader.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"

// todo(ilja): decide
#include "Core/AppContext.h"

using Color = glm::vec3;

struct RenderItem
{
    const Mesh* mesh = nullptr;
    const Object* object = nullptr;
    glm::mat4 pvm{ 1.0f };
    glm::mat4 model{ 1.0f };
    Color color{ 1.0f };
    f32 distanceToCamera = 0.0f;
};

static constexpr i32 MAX_POINT_LIGHTS = 16;
static constexpr i32 MAX_SPOT_LIGHTS = 16;

struct RendererHandles
{
    i32 PVM = -1;
    i32 M = -1;
    i32 V = -1;

    i32 color = -1;

    i32 texDiffuse = -1;
    i32 texSpecular = -1;

    i32 useDiffuseTex = -1;
    i32 useSpecularTex = -1;

    i32 diffuseCol = -1;
    i32 specularCol = -1;
    i32 shininess = -1;

    i32 blendMode = -1;
    i32 alphaCutoff = -1;

    i32 useTextureAtlas = -1;
    i32 atlasColumns = -1;
    i32 atlasRows = -1;
    i32 atlasFrame = -1;

    // fog
    i32 fogMode = -1;
    i32 fogColor = -1;
    i32 fogStart = -1;
    i32 fogEnd = -1;
    i32 fogDensity = -1;

    // directional light
    i32 dirLightDirection = -1;
    i32 dirLightColor = -1;
    i32 dirLightIntensity = -1;

    // pointlight
    i32 pointLightCount = -1;
    i32 pointLightPositions[MAX_POINT_LIGHTS]{};
    i32 pointLightColors[MAX_POINT_LIGHTS]{};
    i32 pointLightIntensities[MAX_POINT_LIGHTS]{};
    i32 pointLightConstants[MAX_POINT_LIGHTS]{};
    i32 pointLightLinears[MAX_POINT_LIGHTS]{};
    i32 pointLightQuadratics[MAX_POINT_LIGHTS]{};

    // spotlight
    i32 spotLightCount = -1;

    i32 spotLightPositions[MAX_SPOT_LIGHTS]{};
    i32 spotLightDirections[MAX_SPOT_LIGHTS]{};
    i32 spotLightColors[MAX_SPOT_LIGHTS]{};
    i32 spotLightIntensities[MAX_SPOT_LIGHTS]{};
    i32 spotLightInnerCutoffCos[MAX_SPOT_LIGHTS]{};
    i32 spotLightOuterCutoffCos[MAX_SPOT_LIGHTS]{};
    i32 spotLightConstants[MAX_SPOT_LIGHTS]{};
    i32 spotLightLinears[MAX_SPOT_LIGHTS]{};
    i32 spotLightQuadratics[MAX_SPOT_LIGHTS]{};
    
    i32 viewPos = -1;
    i32 ambientStrength = -1;

    i32 normalView = -1;
};

struct SkyboxHandles
{
    i32 PV = -1;
    i32 skyboxTex = -1;
};

struct UIHandles
{
    i32 P = -1;
    i32 uiTex = -1;
};

struct DebugHandles
{
    i32 PVM = -1;
    i32 color = -1;
};

struct Renderer
{
    Shader basicShader;
    Shader skyboxShader;
    RendererHandles handles;
    SkyboxHandles skyboxHandles;

    // todo(ilja): think through wherther leave it here
    Shader debugLineShader;
    DebugHandles debugHandles;
    u32 debugCubeVao = 0;
    u32 debugCubeVbo = 0;
    bool showCollisionBoxes = false;
    void init_debug_cube();
    void draw_collision_boxes();
    glm::mat4 build_collision_box_pvm(const Transform& transform, const CollisionBoxComponent& box) const;
    void reload_shaders();
    bool enableCulling = false;
    ////////////

    void init();
    void begin_frame();
    void bind_scene(const Scene& scene);
    void draw();
    void end_frame();

    void draw_model(u32 modelIdx, const Color& color, const Transform& transform);
    void draw_skybox(u32 cubemapTexture, u32 vao);

    // todo(ilja): decide
    AppContext* appContext;

    void toggle_normal_view() const
    {
        i32 currentState;
        glGetUniformiv(basicShader.shaderProgram, handles.normalView, &currentState);
        glUniform1i(handles.normalView, currentState == 0 ? 1 : 0);
    }
    void toggle_collision_boxes_view()
    {
        showCollisionBoxes = !showCollisionBoxes;
    }
    void toggle_culling()
    {
        enableCulling = !enableCulling;
    }

private:
    const Scene* pScene = nullptr;

    f32 get_aspect_ratio() const;

    glm::mat4 build_model_matrix(const Transform& transform) const;
    glm::mat4 build_pvm(const Transform& transform) const;
    void apply_material(const Material& material) const;
    glm::vec3 get_camera_world_position() const;

    bool get_directional_light(glm::vec3& outDir, glm::vec3& outColor, f32& outIntensity) const;
    void upload_point_lights() const;
    void upload_spot_lights() const;
    glm::vec3 get_object_forward_x(const Transform& transform) const;
};