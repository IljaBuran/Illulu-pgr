#include "pch.h"

#include "Common.h"

#include "Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene/Object.h"
#include "Scene/Scene.h"
#include "Renderer/Model.h"
#include "Renderer/ModelDrawable.h"
#include "Renderer/Mesh.h"
#include "Renderer/IDrawable.h"

#include "glad/glad.h"
#include "Scene/ICameraComponent.h"
#include "Scene/DirectionalLightComponent.h"
#include "Scene/PointLightComponent.h"
#include "Scene/SpotLightComponent.h"
#include "Scene/DistanceFogComponent.h"
#include "Scene/AnimatedTextureComponent.h"
#include <cmath>

// todo(ilja): ...
void Renderer::init_debug_cube()
{
    constexpr f32 vertices[] =
    {
        // note(ilja): bottom
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,

        // note(ilja): top
        -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,

        // note(ilja): verticals
        -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
    };

    glGenVertexArrays(1, &debugCubeVao);
    glGenBuffers(1, &debugCubeVbo);

    glBindVertexArray(debugCubeVao);

    glBindBuffer(GL_ARRAY_BUFFER, debugCubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(f32),
        reinterpret_cast<void*>(0)
    );

    glBindVertexArray(0);
}

void Renderer::draw_collision_boxes()
{
    if (!pScene)
        return;

    debugLineShader.use();

    glBindVertexArray(debugCubeVao);

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    // note(ilja): this makes the lines visible through objects, remove if unwanted behaviour
    glDisable(GL_DEPTH_TEST);

    glm::vec3 debugColor(1.0f, 1.0f, 0.0f);
    glUniform3fv(debugHandles.color, 1, glm::value_ptr(debugColor));

    for (const ComponentRef& ref : pScene->cachedCollisionBoxes)
    {
        if (ref.objectIdx >= pScene->objects.size())
            continue;

        const Object& obj = pScene->objects[ref.objectIdx];

        if (ref.componentIdx >= obj.components.size())
            continue;

        const IComponent* comp = obj.components[ref.componentIdx].get();

        if (comp->get_component_kind() != ComponentKind::CollisionBox)
            continue;

        const auto* box = static_cast<const CollisionBoxComponent*>(comp);

        glm::mat4 pvm = build_collision_box_pvm(obj.transform, *box);
        glUniformMatrix4fv(debugHandles.PVM, 1, GL_FALSE, glm::value_ptr(pvm));

        glDrawArrays(GL_LINES, 0, 24);
    }

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    basicShader.use();
}

glm::mat4 Renderer::build_collision_box_pvm(const Transform& transform, const CollisionBoxComponent& box) const
{
    const Object* camObj = pScene->get_current_camera_object();
    const ICameraComponent* cam = pScene->get_current_camera_component();

    if (!camObj || !cam)
        FATAL_CRASH("[RENDERER]: Current camera object/component missing");

    glm::vec3 boxCenter = transform.position + box.offset;

    glm::mat4 model(1.0f);
    model = glm::translate(model, boxCenter);
    model = glm::scale(model, box.size);

    glm::mat4 view = cam->get_view_matrix(camObj->transform);
    glm::mat4 projection = cam->get_projection_matrix(get_aspect_ratio());

    return projection * view * model;
}

void Renderer::reload_shaders()
{
    basicShader.reload();
    skyboxShader.reload();
    debugLineShader.reload();
}

void Renderer::init()
{
    basicShader.load_shader_from_source("Resources/Shaders/Basic.vert", GL_VERTEX_SHADER);
    basicShader.load_shader_from_source("Resources/Shaders/Basic.frag", GL_FRAGMENT_SHADER);
    basicShader.create_and_link_program();

    handles.PVM = basicShader.get_uniform_location("PVM");
    handles.M = basicShader.get_uniform_location("M");
    handles.V = basicShader.get_uniform_location("V");
    handles.color = basicShader.get_uniform_location("color");
    handles.texDiffuse = basicShader.get_uniform_location("texDiffuse");
    handles.texSpecular = basicShader.get_uniform_location("texSpecular");
    handles.useDiffuseTex = basicShader.get_uniform_location("useDiffuseTex");
    handles.useSpecularTex = basicShader.get_uniform_location("useSpecularTex");
    handles.diffuseCol = basicShader.get_uniform_location("materialDiffuseCol");
    handles.specularCol = basicShader.get_uniform_location("materialSpecularCol");
    handles.shininess = basicShader.get_uniform_location("materialShininess");
    handles.blendMode = basicShader.get_uniform_location("materialBlendMode");
    handles.alphaCutoff = basicShader.get_uniform_location("alphaCutoff");

    handles.useTextureAtlas = basicShader.get_uniform_location("useTextureAtlas");
    handles.atlasColumns = basicShader.get_uniform_location("atlasColumns");
    handles.atlasRows = basicShader.get_uniform_location("atlasRows");
    handles.atlasFrame = basicShader.get_uniform_location("atlasFrame");

    handles.fogMode = basicShader.get_uniform_location("fogMode");
    handles.fogColor = basicShader.get_uniform_location("fogColor");
    handles.fogStart = basicShader.get_uniform_location("fogStart");
    handles.fogEnd = basicShader.get_uniform_location("fogEnd");
    handles.fogDensity = basicShader.get_uniform_location("fogDensity");

    handles.dirLightDirection = basicShader.get_uniform_location("dirLightDirection");
    handles.dirLightColor = basicShader.get_uniform_location("dirLightColor");
    handles.dirLightIntensity = basicShader.get_uniform_location("dirLightIntensity");
    
    handles.pointLightCount = basicShader.get_uniform_location("pointLightCount");
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        handles.pointLightPositions[i] =
            basicShader.get_uniform_location(("pointLightPositions[" + std::to_string(i) + "]").c_str());

        handles.pointLightColors[i] =
            basicShader.get_uniform_location(("pointLightColors[" + std::to_string(i) + "]").c_str());

        handles.pointLightIntensities[i] =
            basicShader.get_uniform_location(("pointLightIntensities[" + std::to_string(i) + "]").c_str());

        handles.pointLightConstants[i] =
            basicShader.get_uniform_location(("pointLightConstants[" + std::to_string(i) + "]").c_str());

        handles.pointLightLinears[i] =
            basicShader.get_uniform_location(("pointLightLinears[" + std::to_string(i) + "]").c_str());

        handles.pointLightQuadratics[i] =
            basicShader.get_uniform_location(("pointLightQuadratics[" + std::to_string(i) + "]").c_str());
    }

    handles.spotLightCount = basicShader.get_uniform_location("spotLightCount");

    for (i32 i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        handles.spotLightPositions[i] = 
            basicShader.get_uniform_location(("spotLightPositions[" + std::to_string(i) + "]").c_str());

        handles.spotLightDirections[i] = 
            basicShader.get_uniform_location(("spotLightDirections[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightColors[i] = 
            basicShader.get_uniform_location(("spotLightColors[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightIntensities[i] = 
            basicShader.get_uniform_location(("spotLightIntensities[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightInnerCutoffCos[i] = 
            basicShader.get_uniform_location(("spotLightInnerCutoffCos[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightOuterCutoffCos[i] = 
            basicShader.get_uniform_location(("spotLightOuterCutoffCos[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightConstants[i] = 
            basicShader.get_uniform_location(("spotLightConstants[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightLinears[i] = 
            basicShader.get_uniform_location(("spotLightLinears[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightQuadratics[i] = 
            basicShader.get_uniform_location(("spotLightQuadratics[" + std::to_string(i) + "]").c_str());
    }
    
    handles.viewPos = basicShader.get_uniform_location("viewPos");
    handles.ambientStrength = basicShader.get_uniform_location("ambientStrength");
    
    handles.normalView= basicShader.get_uniform_location("normalView");

    skyboxShader.load_shader_from_source("Resources/Shaders/Skybox.vert", GL_VERTEX_SHADER);
    skyboxShader.load_shader_from_source("Resources/Shaders/Skybox.frag", GL_FRAGMENT_SHADER);
    skyboxShader.create_and_link_program();

    skyboxHandles.PV = skyboxShader.get_uniform_location("PV");
    skyboxHandles.skyboxTex = skyboxShader.get_uniform_location("skybox");

    debugLineShader.load_shader_from_source("Resources/Shaders/DebugLine.vert", GL_VERTEX_SHADER);
    debugLineShader.load_shader_from_source("Resources/Shaders/DebugLine.frag", GL_FRAGMENT_SHADER);
    debugLineShader.create_and_link_program();

    debugHandles.PVM = debugLineShader.get_uniform_location("PVM");
    debugHandles.color = debugLineShader.get_uniform_location("color");

    init_debug_cube();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glDisable(GL_BLEND);

    if (enableCulling)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
}

void Renderer::begin_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    basicShader.use();

    // note(ilja): get camera and upload
    const Object* camObj = pScene->get_current_camera_object();
    const ICameraComponent* cam = pScene->get_current_camera_component();

    if (!camObj || !cam)
        FATAL_CRASH("[RENDERER]: Current camera object/component missing");

    glm::mat4 view = cam->get_view_matrix(camObj->transform);
    glUniformMatrix4fv(handles.V, 1, GL_FALSE, glm::value_ptr(view));

    glUniform1i(handles.texDiffuse, 0);
    glUniform1i(handles.texSpecular, 1);
    glUniform1f(handles.alphaCutoff, 0.5f);

    glm::vec3 camPos = get_camera_world_position();
    glUniform3fv(handles.viewPos, 1, glm::value_ptr(camPos));
    glUniform1f(handles.ambientStrength, 0.12f);

    const DistanceFogComponent* fog = camObj->get_component<DistanceFogComponent>();

    if (fog)
    {
        glUniform1i(handles.fogMode, static_cast<i32>(fog->mode));
        glUniform3fv(handles.fogColor, 1, glm::value_ptr(fog->color));
        glUniform1f(handles.fogStart, fog->start);
        glUniform1f(handles.fogEnd, fog->end);
        glUniform1f(handles.fogDensity, fog->density);
    }
    else
    {
        glUniform1i(handles.fogMode, static_cast<i32>(DistanceFogType::Off));

        glm::vec3 fallbackFogColor(0.55f, 0.60f, 0.65f);
        glUniform3fv(handles.fogColor, 1, glm::value_ptr(fallbackFogColor));
        glUniform1f(handles.fogStart, 20.0f);
        glUniform1f(handles.fogEnd, 120.0f);
        glUniform1f(handles.fogDensity, 0.025f);
    }

    glm::vec3 dir;
    glm::vec3 col;
    f32 intens = 1.0f;

    if (get_directional_light(dir, col, intens))
    {
        glUniform3fv(handles.dirLightDirection, 1, glm::value_ptr(dir));
        glUniform3fv(handles.dirLightColor, 1, glm::value_ptr(col));
        glUniform1f(handles.dirLightIntensity, intens);
    }
    else
    {
        glm::vec3 fallbackDir = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
        glm::vec3 fallbackColor(1.0f, 1.0f, 1.0f);

        glUniform3fv(handles.dirLightDirection, 1, glm::value_ptr(fallbackDir));
        glUniform3fv(handles.dirLightColor, 1, glm::value_ptr(fallbackColor));
        glUniform1f(handles.dirLightIntensity, 1.0f);
    }

    // todo(ilja): inefficient, redo later
    upload_point_lights();
    upload_spot_lights();
}

void Renderer::bind_scene(const Scene& scene)
{
    pScene = &scene;
}

void Renderer::draw()
{
    if (!pScene)
        FATAL_CRASH("[RENDERER]: No scene binded during Renderer::draw()!");

    
    std::vector<RenderItem> opaqueItems;
    std::vector<RenderItem> transparentItems;

    glm::vec3 camPos = get_camera_world_position();

    for (const auto& compRef : pScene->cachedDrawables)
    {
        const Object& obj = pScene->objects[compRef.objectIdx];
        const IDrawable* drawable = static_cast<const IDrawable*>(obj.components[compRef.componentIdx].get());

        if (!drawable->visible)
            continue;

        if (drawable->get_component_kind() == ComponentKind::DrawableSkybox)
        {
            drawable->draw(*this, obj.transform);
            continue;
        }

        if (drawable->get_component_kind() != ComponentKind::DrawableModel)
            continue;

        const ModelDrawable* modelDrawable = static_cast<const ModelDrawable*>(drawable);
        if (modelDrawable->modelIdx >= pScene->models.size())
            continue;

        const Model& model = pScene->models[modelDrawable->modelIdx];
        glm::mat4 modelMat = build_model_matrix(obj.transform);
        glm::mat4 pvm = build_pvm(obj.transform);

        f32 objectDistance = glm::length(camPos - obj.transform.position);

        for (const Mesh& mesh : model.meshes)
        {
            RenderItem item;
            item.mesh = &mesh;
            item.object = &obj;
            item.model = modelMat;
            item.pvm = pvm;
            item.color = modelDrawable->color;
            item.distanceToCamera = objectDistance;

            if (mesh.material.blendMode == BlendMode::Transparent)
                transparentItems.push_back(item);
            else
                opaqueItems.push_back(item);
        }
    }

    // note(ilja): opaque and masked first
    basicShader.use();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    if (enableCulling)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    for (const RenderItem& item : opaqueItems)
    {
        glUniformMatrix4fv(handles.PVM, 1, GL_FALSE, glm::value_ptr(item.pvm));
        glUniformMatrix4fv(handles.M, 1, GL_FALSE, glm::value_ptr(item.model));
        glUniform3fv(handles.color, 1, glm::value_ptr(item.color));

        // todo(ilja): ineffective, but ok for now
        const AnimatedTextureComponent* anim = item.object->get_component<AnimatedTextureComponent>();
        if (anim && anim->columns > 0 && anim->rows > 0 && anim->frameCount > 1)
        {
            glUniform1i(handles.useTextureAtlas, 1);
            glUniform1i(handles.atlasColumns, anim->columns);
            glUniform1i(handles.atlasRows, anim->rows);
            glUniform1i(handles.atlasFrame, anim->currentFrame);
        }
        else
        {
            glUniform1i(handles.useTextureAtlas, 0);
            glUniform1i(handles.atlasColumns, 1);
            glUniform1i(handles.atlasRows, 1);
            glUniform1i(handles.atlasFrame, 0);
        }

        apply_material(item.mesh->material);
        item.mesh->draw();
    }

    // note(ilja): transparent back-to-front
    std::sort(
        transparentItems.begin(),
        transparentItems.end(),
        [](const RenderItem& a, const RenderItem& b)
        {
            return a.distanceToCamera > b.distanceToCamera;
        }
    );

    
    if (enableCulling)
        glDisable(GL_CULL_FACE);

    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (const RenderItem& item : transparentItems)
    {
        glUniformMatrix4fv(handles.PVM, 1, GL_FALSE, glm::value_ptr(item.pvm));
        glUniformMatrix4fv(handles.M, 1, GL_FALSE, glm::value_ptr(item.model));
        glUniform3fv(handles.color, 1, glm::value_ptr(item.color));

        apply_material(item.mesh->material);
        item.mesh->draw();
    }

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    if (showCollisionBoxes)
        draw_collision_boxes();
}

void Renderer::end_frame()
{}

void Renderer::draw_model(u32 modelIdx, const Color& color, const Transform& transform)
{
    if (!pScene)
        FATAL_CRASH("[RENDERER]: No scene binded during Renderer::draw_model()!");

    if (modelIdx >= pScene->models.size())
        FATAL_CRASH("[RENDERER]: Trying to render object with invalid modelIdx");

    basicShader.use();

    const Model& model = pScene->models[modelIdx];
    glm::mat4 modelMat = build_model_matrix(transform);
    glm::mat4 pvm = build_pvm(transform);

    glUniformMatrix4fv(handles.PVM, 1, GL_FALSE, glm::value_ptr(pvm));
    glUniformMatrix4fv(handles.M, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniform3fv(handles.color, 1, glm::value_ptr(color));

    for (const Mesh& mesh : model.meshes)
    {
        apply_material(mesh.material);
        mesh.draw();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Renderer::draw_skybox(u32 cubemapTexture, u32 vao)
{
    if (!pScene)
        FATAL_CRASH("[RENDERER]: No scene binded during Renderer::draw_skybox()!");
    // todo(ilja): inefficient
    const Object* camObj = pScene->get_current_camera_object();
    const ICameraComponent* cam = pScene->get_current_camera_component();

    if (!camObj || !cam)
        FATAL_CRASH("[RENDERER]: Current camera object/component missing");

    if (enableCulling)
        glDisable(GL_CULL_FACE);

    glm::mat4 view = glm::mat4(glm::mat3(cam->get_view_matrix(camObj->transform)));
    glm::mat4 PV = cam->get_projection_matrix(get_aspect_ratio()) * view;

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    skyboxShader.use();
    glUniformMatrix4fv(skyboxHandles.PV, 1, GL_FALSE, glm::value_ptr(PV));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(skyboxHandles.skyboxTex, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    if (enableCulling)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    basicShader.use();
    glUniform1i(handles.texDiffuse, 0);
}

glm::vec3 Renderer::get_object_forward_x(const Transform& transform) const
{
    glm::mat4 R(1.0f);

    R = glm::rotate(R, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    R = glm::rotate(R, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    R = glm::rotate(R, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 localForward(1.0f, 0.0f, 0.0f);
    glm::vec3 worldForward = glm::vec3(R * glm::vec4(localForward, 0.0f));

    return glm::normalize(worldForward);
}

f32 Renderer::get_aspect_ratio() const
{
    f32 res = static_cast<f32>(appContext->pWindow->width) / static_cast<f32>(appContext->pWindow->height);
    // note(ilja): this might sometimes be nan, for example after returning from minimized windows (minimized window -> framebuffer 0x0)
    return std::isnan(res) ? 16.0f / 9.0f : res;
}

glm::mat4 Renderer::build_model_matrix(const Transform& transform) const
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, transform.position);
    model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);

    return model;
}

glm::mat4 Renderer::build_pvm(const Transform& transform) const
{
    const Object* camObj = pScene->get_current_camera_object();
    const ICameraComponent* cam = pScene->get_current_camera_component();

    if (!camObj || !cam)
        FATAL_CRASH("[RENDERER]: Current camera object/component missing");

    glm::mat4 model = build_model_matrix(transform);
    glm::mat4 view = cam->get_view_matrix(camObj->transform);
    return cam->get_projection_matrix(get_aspect_ratio()) * view * model;
}

void Renderer::apply_material(const Material& material) const
{
    glUniform1i(handles.useDiffuseTex, material.diffuseTex != 0 ? 1 : 0);
    glUniform1i(handles.useSpecularTex, material.specularTex != 0 ? 1 : 0);

    glUniform4fv(handles.diffuseCol, 1, glm::value_ptr(material.diffuseCol));
    glUniform3fv(handles.specularCol, 1, glm::value_ptr(material.specularCol));
    glUniform1f(handles.shininess, material.shininess);
    glUniform1i(handles.blendMode, static_cast<i32>(material.blendMode));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuseTex);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.specularTex);
}

glm::vec3 Renderer::get_camera_world_position() const
{
    const Object* camObj = pScene->get_current_camera_object();
    const ICameraComponent* cam = pScene->get_current_camera_component();

    if (!camObj || !cam)
        FATAL_CRASH("[RENDERER]: Current camera object / component missing");

    return camObj->transform.position;
}

bool Renderer::get_directional_light(glm::vec3& outDir, glm::vec3& outColor, f32& outIntensity) const
{
    if (!pScene)
        return false;

    for (const ComponentRef& ref : pScene->cachedLights)
    {
        const Object& obj = pScene->objects[ref.objectIdx];
        const IComponent* comp = obj.components[ref.componentIdx].get();

        if (comp->get_component_kind() != ComponentKind::DirectionalLight)
            continue;

        const DirectionalLightComponent* light = static_cast<const DirectionalLightComponent*>(comp);

        outDir = get_object_forward_x(obj.transform);
        outColor = light->color;
        outIntensity = light->intensity;
        return true;
    }

    return false;
}

void Renderer::upload_point_lights() const
{
    if (!pScene)
    {
        glUniform1i(handles.pointLightCount, 0);
        return;
    }

    i32 count = 0;

    for (const ComponentRef& ref : pScene->cachedLights)
    {
        if (count >= MAX_POINT_LIGHTS)
            break;

        const Object& obj = pScene->objects[ref.objectIdx];
        const IComponent* comp = obj.components[ref.componentIdx].get();

        if (comp->get_component_kind() != ComponentKind::PointLight)
            continue;

        const PointLightComponent* light = static_cast<const PointLightComponent*>(comp);

        glUniform3fv(handles.pointLightPositions[count], 1, glm::value_ptr(obj.transform.position));
        glUniform3fv(handles.pointLightColors[count], 1, glm::value_ptr(light->color));
        glUniform1f(handles.pointLightIntensities[count], light->intensity);
        glUniform1f(handles.pointLightConstants[count], light->constant);
        glUniform1f(handles.pointLightLinears[count], light->linear);
        glUniform1f(handles.pointLightQuadratics[count], light->quadratic);

        count++;
    }

    glUniform1i(handles.pointLightCount, count);
}

void Renderer::upload_spot_lights() const
{
    if (!pScene)
    {
        glUniform1i(handles.spotLightCount, 0);
        return;
    }

    i32 count = 0;

    for (const ComponentRef& ref : pScene->cachedLights)
    {
        if (count >= MAX_SPOT_LIGHTS)
        {
            break;
        }

        const Object& obj = pScene->objects[ref.objectIdx];
        const IComponent* comp = obj.components[ref.componentIdx].get();

        if (comp->get_component_kind() != ComponentKind::SpotLight)
            continue;

        const SpotLightComponent* light = static_cast<const SpotLightComponent*>(comp);
        glm::vec3 direction = get_object_forward_x(obj.transform);

        glUniform3fv(handles.spotLightPositions[count], 1, glm::value_ptr(obj.transform.position));
        glUniform3fv(handles.spotLightDirections[count], 1, glm::value_ptr(direction));
        glUniform3fv(handles.spotLightColors[count], 1, glm::value_ptr(light->color));
        glUniform1f(handles.spotLightIntensities[count], light->intensity);
        glUniform1f(handles.spotLightInnerCutoffCos[count], glm::cos(glm::radians(light->innerCutoffDeg)));
        glUniform1f(handles.spotLightOuterCutoffCos[count], glm::cos(glm::radians(light->outerCutoffDeg)));
        glUniform1f(handles.spotLightConstants[count], light->constant);
        glUniform1f(handles.spotLightLinears[count], light->linear);
        glUniform1f(handles.spotLightQuadratics[count], light->quadratic);

        count++;
    }

    glUniform1i(handles.spotLightCount, count);
}