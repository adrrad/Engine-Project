#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"
#include "utilities/Utilities.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/LightComponent.hpp"
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

SceneObject* CreatePlane(Shader* PBRShader, string name = "Sphere")
{
    Mesh* sphere = Mesh::GetPlane(2,2, PBRShader, 250.0f); //Mesh::GetSphere(PBRShader);
    SceneObject* object = new SceneObject();
    MeshComponent* meshcomp = object->AddComponent<MeshComponent>();
    Material* mat = PBRShader->CreateMaterial();
    Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    // Texture* ao =       Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Planks16\\Planks16_AO.jpg"), GL_TEXTURE_2D);
    object->Name = name;
    object->transform.position = {50.0f, 3.5f, 50.0f};
    mat->SetTexture("albedo", albedo);
    mat->SetTexture("metallic", metallic);
    mat->SetTexture("roughness", roughness);
    // mat->SetTexture("ambient", ao);
    // mat->GetUniform("hasAO")->i = 0;
    // mat->SetNormalMap(normal);
    // mat->GetUniform("PBR.Albedo")->f4 = {0.8f, 0.8f, 0.8f, 1.0f};
    // mat->GetUniform("PBR.Roughness")->f = 0.550f;
    // mat->GetUniform("PBR.Metallic")->f = 0.630f;
    // mat->GetUniform("PBR.F0")->f4 = {0.24, 0.24, 0.24, 1.0f};
    // mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.0;
    meshcomp->SetMesh(sphere);
    meshcomp->SetMaterial(mat);

    return object;
}

SceneObject* CreateSphere(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    static Mesh* sphereMesh =  Mesh::GetSphere(shader);

    SceneObject* sphere = new SceneObject();
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    mp->SetMesh(sphereMesh);
    Material* mat = shader->CreateMaterial();
    vec3 f = {0.24, 0.24, 0.24};
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->SetTexture("textures.albedo", albedo);
    mat->SetTexture("textures.metallic", metallic);
    mat->SetTexture("textures.roughness", roughness);
    mat->SetTexture("textures.normal", normal);
    mp->SetMaterial(mat);
    return sphere;
}


SceneObject* CreateTerrain(std::string path, vec3 position, vec3 rotation, Shader* PBRShader)
{
    static Mesh* sphere = Mesh::FromHeightmap(path, 1000.0f, 255.0f, PBRShader);
    SceneObject* object = new SceneObject();
    MeshComponent* meshcomp = object->AddComponent<MeshComponent>();
    Material* mat = PBRShader->CreateMaterial();
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_nrm.jpg"), GL_TEXTURE_2D);
    object->Name = "Terrain";
    object->transform.position = position;
    object->transform.rotation = rotation;
    mat->SetTexture("albedo", albedo);
    mat->SetTexture("metallic", metallic);
    mat->SetTexture("roughness", roughness);
    // mat->GetUniform("hasAO")->i = 0;
    // mat->SetNormalMap(normal);
    // mat->GetUniform("PBR.Albedo")->f4 = {0.8f, 0.8f, 0.8f, 1.0f};
    // mat->GetUniform("PBR.Roughness")->f = 0.550f;
    // mat->GetUniform("PBR.Metallic")->f = 0.630f;
    // mat->GetUniform("PBR.F0")->f4 = {0.24, 0.24, 0.24, 1.0f};
    // mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.0;
    meshcomp->SetMesh(sphere);
    meshcomp->SetMaterial(mat);
    return object;
}


SceneObject* CreatePointLight(vec3 position, vec4 colour, float radius)
{
    SceneObject* pointlight = new SceneObject();
    pointlight->Name = "Point Light";
    pointlight->transform.position = position;
    auto plight = pointlight->AddComponent<LightComponent>();
    plight->SetType(LightType::POINT);
    plight->PointLight().Colour = colour;
    plight->PointLight().Radius = radius;
    return pointlight;
}

SceneObject* CreateDirectionalLight(vec4 colour)
{
    SceneObject* light = new SceneObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = {-135.0f, 0.0f, 0.0f};
    auto lcomp = light->AddComponent<LightComponent>();
    lcomp->SetType(LightType::DIRECTIONAL);
    lcomp->SetColour(colour);
    return light;
}

int scene2()
{
    Renderer* renderer = Renderer::GetInstance();
    Scene scene = Scene();

    SceneObject* cameraObject = new SceneObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 0, 5);
    auto cam = cameraObject->AddComponent<CameraComponent>();
    
    SceneObject* quad = new SceneObject();
    Shader* shader = Shader::Create("heheheh").Build();
    // Shader* shader = Shader(GetAbsoluteResourcesPath("\\shaders\\testing\\vertex.vert"), GetAbsoluteResourcesPath("\\shaders\\testing\\fragment.frag"))
    shader->AllocateBuffers(10);
    auto sphere1 = CreateSphere({-3,0,0}, shader);
    auto sphere3 = CreateSphere({3,0,0}, shader);
    auto sphere2 = CreateSphere({0,0,0}, shader);
    auto p = CreatePointLight({5,0,0}, {1.0f, 1.0f, 0.0f, 1.0f}, 50.0f);
    auto p2 = CreatePointLight({-5,0,0}, {1.0f, 1.0f, 1.0f, 1.0f}, 50.0f);
    auto d = CreateDirectionalLight(vec4(1));

    scene.AddSceneObject(p);
    scene.AddSceneObject(p2);
    scene.AddSceneObject(d);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(sphere1);
    scene.AddSceneObject(sphere2);
    scene.AddSceneObject(sphere3);
    auto winMan = WindowManager::GetInstance();
    winMan->UnlockCursor(winMan->GetActiveWindow());

    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}

int main()
{
    scene2();
}