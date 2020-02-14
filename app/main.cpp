#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"
#include "utilities/Utilities.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/DirectionalLightComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/WaveManagerComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "SlerpComponent.hpp"

#include <iostream>


using namespace std;
using namespace glm;
using namespace Rendering;
using namespace Components;
using namespace Utilities;

SceneObject* CreateSphere()
{
    Shader* PBRShader = Shader::GetTexturesPBRShader();
    Mesh* sphere = Mesh::GetPlane(2,2, PBRShader, 250.0f); //Mesh::GetSphere(PBRShader);
    SceneObject* object = new SceneObject();
    MeshComponent* meshcomp = object->AddComponent<MeshComponent>();
    Material* mat = new Material(PBRShader);
    Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    object->Name = "Sphere";
    object->transform.position = {0.0f, 5.0f, 5.0f};
    mat->SetTexture("albedo", albedo);
    mat->SetTexture("metallic", metallic);
    mat->SetTexture("roughness", roughness);
    mat->SetNormalMap(normal);
    // mat->GetUniform("PBR.Albedo")->f4 = {0.8f, 0.8f, 0.8f, 1.0f};
    // mat->GetUniform("PBR.Roughness")->f = 0.550f;
    // mat->GetUniform("PBR.Metallic")->f = 0.630f;
    mat->GetUniform("PBR.F0")->f4 = {0.24, 0.24, 0.24, 1.0f};
    mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.0;
    meshcomp->SetMesh(sphere);
    meshcomp->SetMaterial(mat);

    return object;
}

SceneObject* CreateCube(vec3 position, vec3 rotation)
{
    static Shader* PBRShader = Shader::GetTexturesPBRShader();
    static Mesh* sphere = Mesh::GetCube(PBRShader); //Mesh::GetSphere(PBRShader);
    SceneObject* object = new SceneObject();
    MeshComponent* meshcomp = object->AddComponent<MeshComponent>();
    Material* mat = new Material(PBRShader);
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    object->Name = "Sphere";
    object->transform.position = position;
    object->transform.rotation = rotation;
    mat->SetTexture("albedo", albedo);
    mat->SetTexture("metallic", metallic);
    mat->SetTexture("roughness", roughness);
    mat->SetNormalMap(normal);
    // mat->GetUniform("PBR.Albedo")->f4 = {0.8f, 0.8f, 0.8f, 1.0f};
    // mat->GetUniform("PBR.Roughness")->f = 0.550f;
    // mat->GetUniform("PBR.Metallic")->f = 0.630f;
    mat->GetUniform("PBR.F0")->f4 = {0.24, 0.24, 0.24, 1.0f};
    mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.0;
    meshcomp->SetMesh(sphere);
    meshcomp->SetMaterial(mat);

    return object;
}

int main()
{
    Renderer* renderer = Renderer::GetInstance();
    Scene scene = Scene();
    Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\back.tga"));
    Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\front.tga"));
    Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\left.tga"));
    Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\right.tga"));
    Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\top.tga"));
    Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sand_bot2.jpg"));
    Texture* skyboxTexture = new Texture(right, left, top, bot, back, front);

    Shader* gerstnerShader = Shader::GetGerstnerWaveShader_PBR();
    Mesh* waveMesh = Mesh::GetPlane(1000, 1000, gerstnerShader, 100.0f);
    

    Texture* normalMap = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\water_norm_map.png"), GL_TEXTURE_2D);

    Texture* albedo = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\aa_beauty_and_the_sun.png"), GL_TEXTURE_2D);
    SceneObject *obj = new SceneObject();
    obj->Name = "Water";
    auto mp = obj->AddComponent<MeshComponent>();
    auto wm = obj->AddComponent<WaveManagerComponent>();

    wm->AddDirectionalWave(105.0f, 1.0f, 10.0f, 10.0f);
    wm->AddDirectionalWave(155.0f, 1.0f, 10.0f, 10.0f);
    wm->AddDirectionalWave(125.0f, 1.0f, 10.0f, 10.0f);
    wm->AddDirectionalWave(25.0f, 1.0f, 10.0f, 10.0f);


    mp->SetMesh(waveMesh);
    Material* mat = new Material(gerstnerShader);
    mat->SetNormalMap(normalMap);
    mat->SetTexture("albedo", albedo);
    if(mat->GetUniform("PBR.Albedo") != nullptr)
    {
        mat->GetUniform("PBR.Albedo")->f4 = {0.3f, 0.3f, 0.3f, 1.0f};
        mat->GetUniform("PBR.Roughness")->f = 0.250;
        mat->GetUniform("PBR.Metallic")->f = 0.630;
        mat->GetUniform("PBR.F0")->f4 = {0.2f, 0.2f, 0.2f, 1.0f};
        mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.230f;
    }

    mp->SetMaterial(mat);
    wm->SetGerstnerMaterial(mat);

    SceneObject* cameraObject = new SceneObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 5, 0);
    auto cam = cameraObject->AddComponent<CameraComponent>();


    auto skybox = cameraObject->AddComponent<SkyboxComponent>();
    skybox->SetSkyboxTexture(skyboxTexture);

    
    auto mov = cameraObject->AddComponent<MovementComponent>();
    mov->SetCamera(cam);
    mov->SetWaveManager(wm);
    SceneObject* light = new SceneObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = {-135.0f, 0.0f, 0.0f};
    auto lcomp = light->AddComponent<DirectionalLightComponent>();
    

    auto cube1 = CreateCube(vec3(-5.0f, 5.0f, 0.0f), vec3(0.0f, -45.0f, 0.0f));
    auto cube2 = CreateCube(vec3(5.0f, 5.0f, 0.0f), vec3(0.0f, 45.0f, 0.0f));
    auto cube3 = CreateCube(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));

    auto slerp = cube3->AddComponent<SlerpComponent>();
    slerp->SetTransforms(&cube1->transform, &cube2->transform);

    scene.AddSceneObject(cube1);
    scene.AddSceneObject(cube2);
    scene.AddSceneObject(cube3);
    
    lcomp->SetDebugDrawDirectionEnabled();
    scene.AddSceneObject(obj);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(light);
    
    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}