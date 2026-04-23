#include "pch.h"

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
#include "Scene/DirectionalLightComponent.h"
#include "Scene/PointLightComponent.h"
#include "Scene/SpotLightComponent.h"

void Renderer::init()
{
    shader.load_shader_from_source("Resources/Shaders/Basic.vert", GL_VERTEX_SHADER);
    shader.load_shader_from_source("Resources/Shaders/Basic.frag", GL_FRAGMENT_SHADER);
    shader.create_and_link_program();

    handles.PVM = shader.get_uniform_location("PVM");
    handles.M = shader.get_uniform_location("M");
    handles.color = shader.get_uniform_location("color");
    handles.texDiffuse = shader.get_uniform_location("texDiffuse");
    handles.texSpecular = shader.get_uniform_location("texSpecular");
    handles.useDiffuseTex = shader.get_uniform_location("useDiffuseTex");
    handles.useSpecularTex = shader.get_uniform_location("useSpecularTex");
    handles.diffuseCol = shader.get_uniform_location("materialDiffuseCol");
    handles.specularCol = shader.get_uniform_location("materialSpecularCol");
    handles.shininess = shader.get_uniform_location("materialShininess");
    handles.blendMode = shader.get_uniform_location("materialBlendMode");
    handles.alphaCutoff = shader.get_uniform_location("alphaCutoff");
    handles.dirLightDirection = shader.get_uniform_location("dirLightDirection");
    handles.dirLightColor = shader.get_uniform_location("dirLightColor");
    handles.dirLightIntensity = shader.get_uniform_location("dirLightIntensity");
    
    handles.pointLightCount = shader.get_uniform_location("pointLightCount");
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        handles.pointLightPositions[i] =
            shader.get_uniform_location(("pointLightPositions[" + std::to_string(i) + "]").c_str());

        handles.pointLightColors[i] =
            shader.get_uniform_location(("pointLightColors[" + std::to_string(i) + "]").c_str());

        handles.pointLightIntensities[i] =
            shader.get_uniform_location(("pointLightIntensities[" + std::to_string(i) + "]").c_str());

        handles.pointLightConstants[i] =
            shader.get_uniform_location(("pointLightConstants[" + std::to_string(i) + "]").c_str());

        handles.pointLightLinears[i] =
            shader.get_uniform_location(("pointLightLinears[" + std::to_string(i) + "]").c_str());

        handles.pointLightQuadratics[i] =
            shader.get_uniform_location(("pointLightQuadratics[" + std::to_string(i) + "]").c_str());
    }

    handles.spotLightCount = shader.get_uniform_location("spotLightCount");

    for (i32 i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        handles.spotLightPositions[i] = 
            shader.get_uniform_location(("spotLightPositions[" + std::to_string(i) + "]").c_str());

        handles.spotLightDirections[i] = 
            shader.get_uniform_location(("spotLightDirections[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightColors[i] = 
            shader.get_uniform_location(("spotLightColors[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightIntensities[i] = 
            shader.get_uniform_location(("spotLightIntensities[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightInnerCutoffCos[i] = 
            shader.get_uniform_location(("spotLightInnerCutoffCos[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightOuterCutoffCos[i] = 
            shader.get_uniform_location(("spotLightOuterCutoffCos[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightConstants[i] = 
            shader.get_uniform_location(("spotLightConstants[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightLinears[i] = 
            shader.get_uniform_location(("spotLightLinears[" + std::to_string(i) + "]").c_str());
        
        handles.spotLightQuadratics[i] = 
            shader.get_uniform_location(("spotLightQuadratics[" + std::to_string(i) + "]").c_str());
    }
    
    handles.viewPos = shader.get_uniform_location("viewPos");
    handles.ambientStrength = shader.get_uniform_location("ambientStrength");

    skyboxShader.load_shader_from_source("Resources/Shaders/Skybox.vert", GL_VERTEX_SHADER);
    skyboxShader.load_shader_from_source("Resources/Shaders/Skybox.frag", GL_FRAGMENT_SHADER);
    skyboxShader.create_and_link_program();

    skyboxHandles.VP = skyboxShader.get_uniform_location("VP");
    skyboxHandles.skyboxTex = skyboxShader.get_uniform_location("skybox");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glDisable(GL_BLEND);
}

void Renderer::begin_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();

    glUniform1i(handles.texDiffuse, 0);
    glUniform1i(handles.texSpecular, 1);
    glUniform1f(handles.alphaCutoff, 0.5f);

    glm::vec3 camPos = get_camera_world_position();
    glUniform3fv(handles.viewPos, 1, glm::value_ptr(camPos));
    glUniform1f(handles.ambientStrength, 0.12f);

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

    // opaque + masked first
    shader.use();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    for (const RenderItem& item : opaqueItems)
    {
        glUniformMatrix4fv(handles.PVM, 1, GL_FALSE, glm::value_ptr(item.pvm));
        glUniformMatrix4fv(handles.M, 1, GL_FALSE, glm::value_ptr(item.model));
        glUniform3fv(handles.color, 1, glm::value_ptr(item.color));

        apply_material(item.mesh->material);
        item.mesh->draw();
    }

    // transparent back-to-front
    std::sort(
        transparentItems.begin(),
        transparentItems.end(),
        [](const RenderItem& a, const RenderItem& b)
        {
            return a.distanceToCamera > b.distanceToCamera;
        }
    );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glDisable(GL_BLEND);
}

void Renderer::end_frame()
{}

void Renderer::draw_model(u32 modelIdx, const Color& color, const Transform& transform)
{
    if (!pScene)
        FATAL_CRASH("[RENDERER]: No scene binded during Renderer::draw_model()!");

    if (modelIdx >= pScene->models.size())
        FATAL_CRASH("[RENDERER]: Trying to render object with invalid modelIdx");

    shader.use();

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

    glm::mat4 view = glm::mat4(glm::mat3(cam->get_view_matrix(camObj->transform)));
    glm::mat4 PV = cam->get_projection_matrix(get_aspect_ratio()) * view;

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    skyboxShader.use();
    glUniformMatrix4fv(skyboxHandles.VP, 1, GL_FALSE, glm::value_ptr(PV));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(skyboxHandles.skyboxTex, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    shader.use();
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
    return static_cast<f32>(appContext->pWindow->width) / static_cast<f32>(appContext->pWindow->height);
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

    glUniform3fv(handles.diffuseCol, 1, glm::value_ptr(material.diffuseCol));
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
