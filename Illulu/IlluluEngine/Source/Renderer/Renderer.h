#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "Renderer/Shader.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"
#include "Scene/Transform.h"

using Color = glm::vec3;

struct RenderItem
{
    const Mesh* mesh = nullptr;
    glm::mat4 pvm{ 1.0f };
    glm::mat4 model{ 1.0f };
    Color color{ 1.0f };
    f32 distanceToCamera = 0.0f;
};

static constexpr i32 MAX_POINT_LIGHTS = 8;

struct RendererHandles
{
    i32 PVM = -1;
    i32 M = -1;

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

    i32 dirLightDirection = -1;
    i32 dirLightColor = -1;
    i32 dirLightIntensity = -1;

    i32 pointLightCount = -1;
    i32 pointLightPositions[MAX_POINT_LIGHTS];
    i32 pointLightColors[MAX_POINT_LIGHTS];
    i32 pointLightIntensities[MAX_POINT_LIGHTS];
    i32 pointLightConstants[MAX_POINT_LIGHTS];
    i32 pointLightLinears[MAX_POINT_LIGHTS];
    i32 pointLightQuadratics[MAX_POINT_LIGHTS];

    i32 viewPos = -1;
    i32 ambientStrength = -1;
};

struct SkyboxHandles
{
    i32 VP = -1;
    i32 skyboxTex = -1;
};

struct Renderer
{
    Shader shader;
    Shader skyboxShader;

    RendererHandles handles;
    SkyboxHandles skyboxHandles;

    void init();
    void begin_frame();
    void bind_scene(const Scene& scene);
    void draw();
    void end_frame();

    void draw_model(u32 modelIdx, const Color& color, const Transform& transform);
    void draw_skybox(u32 cubemapTexture, u32 vao);

private:
    const Scene* pScene = nullptr;

    glm::mat4 build_model_matrix(const Transform& transform) const;
    glm::mat4 build_pvm(const Transform& transform) const;
    void apply_material(const Material& material) const;
    glm::vec3 get_camera_world_position() const;

    bool get_main_directional_light(glm::vec3& outDir, glm::vec3& outColor, f32& outIntensity) const;
    void upload_point_lights() const;
    glm::vec3 get_object_forward_x(const Transform& transform) const;

    glm::mat4 projection = glm::perspective(glm::radians(75.0f), 1920.0f / 1080.0f, 0.1f, 180.0f);
};