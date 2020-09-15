#include "rendering/Shader.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/Debugging.hpp"
#include "utilities/Utilities.hpp"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>



using namespace std;
using namespace Engine::Utilities;

namespace Engine::Rendering
{
typedef Shader::ShaderBuilder ShaderBuilder;

Shader::Shader(std::string name, std::string vertexSource, std::string fragmentSource, std::vector<GLSLStruct*> blocks, std::vector<std::string> textures)
{
    _name = name;
    _vertexSource = vertexSource;
    _fragmentSource = fragmentSource;
    _textures = textures;

    for(auto block : blocks)
    {
        _uniformBlocks[block->Name] = block;
    }

    CompileShader();
    Renderer::GetInstance()->AddShader(this);
}

Shader::Shader(string vertexPath, string fragmentPath)
{       
    _vFiles = {vertexPath};
    _fFiles = {fragmentPath};
    ifstream vShaderFile;
    ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

    for(string vpath : _vFiles)
    {
        string code = Utilities::ReadFile(vpath);
        _vertexSource += code.c_str();
    }
    for(string fpath : _fFiles)
    {
        string code = Utilities::ReadFile(fpath);
        _fragmentSource += code.c_str();
    }
    CompileShader();
    Renderer::GetInstance()->AddShader(this);
}

Shader::Shader(vector<string> vertexPath, vector<string> fragmentPath)
{
    _vFiles = vertexPath;
    _fFiles = fragmentPath;
    ifstream vShaderFile;
    ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

    for(string vpath : _vFiles)
    {
        string code = Utilities::ReadFile(vpath);
        _vertexSource += code.c_str();
    }
    for(string fpath : _fFiles)
    {
        string code = Utilities::ReadFile(fpath);
        _fragmentSource += code.c_str();
    }
    CompileShader();
    Renderer::GetInstance()->AddShader(this);
}


void Shader::CompileShader()
{
    
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vsrc = _vertexSource.c_str();
    glShaderSource(vertex, 1, &vsrc, NULL);
    glCompileShader(vertex);
    CheckShaderStatus(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fsrc = _fragmentSource.c_str();
    glShaderSource(fragment, 1, &fsrc, NULL);
    glCompileShader(fragment);
    CheckShaderStatus(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    CheckShaderStatus(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    int count = 0;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &count);
}

void Shader::AllocateBuffers(ElementCount numInstances)
{
    if(_maxInstances != 0) throw std::exception("Shader resrources allocation failed! Already allocated!");
    for(auto& str : _uniformBlocks)
    {
        str.second->Allocate(numInstances);
    }
    _maxInstances = numInstances;
}

bool Shader::RecompileShader()
{
    _vertexSource = "";
    _fragmentSource = "";
    if(_vFiles.size() == 0 || _fFiles.size() == 0) throw std::exception("Cannot recompile a procedurally generated shader!");
    for(string vpath : _vFiles)
    {
        string code = Utilities::ReadFile(vpath);
        _vertexSource += code.c_str();
    }
    for(string fpath : _fFiles)
    {
        string code = Utilities::ReadFile(fpath);
        _fragmentSource += code.c_str();
    }

    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vsrc = _vertexSource.c_str();
    glShaderSource(vertex, 1, &vsrc, NULL);
    glCompileShader(vertex);
    bool status;
    status = CheckShaderStatus(vertex, "VERTEX");
    if(!status) return false;
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fsrc = _fragmentSource.c_str();
    glShaderSource(fragment, 1, &fsrc, NULL);
    glCompileShader(fragment);
    status = CheckShaderStatus(fragment, "FRAGMENT", false);
    if(!status) return false;
    // shader Program
    glDeleteProgram(ID);
    uint32_t newID = glCreateProgram();
    glAttachShader(newID, vertex);
    glAttachShader(newID, fragment);
    glLinkProgram(newID);
    status = CheckShaderStatus(newID, "PROGRAM", false);
    if(!status) return false; // TODO: Delete shader + program
    ID = newID;
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

void Shader::UpdateUniformBuffers()
{
    for(auto ub : _uniformBlocks) ub.second->UpdateUniformBuffer();
}

bool Shader::CheckShaderStatus(uint32_t shader, string type, bool stopOnFailure)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            string msg = string(_name + " shader compilation error: ") + type + string("\n") + string(infoLog);
            cout << msg << endl;
            if(stopOnFailure) throw exception("ERROR::SHADER_COMPILATION_ERROR");
            return false;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            string msg = string("ERROR::PROGRAM_LINKING_ERROR of type: ") + type + string("\n") + string(infoLog);
            cout << msg << endl;
            if(stopOnFailure) throw exception("ERROR::PROGRAM_LINKING_ERROR");
            return false;
        }
    }
    return true;
}


int Shader::ULoc(string name)
{
    return glGetUniformLocation(ID,name.c_str());
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::Use()
{
    glUseProgram(ID);
}

uint32_t Shader::GetID()
{
    return ID;
}

Material* Shader::CreateMaterial()
{
    if(_maxInstances == 0) throw std::exception("Cannot create material! No buffers allocated!");
    if(_maxInstances <= _numInstances) throw std::exception("Cannot create material! Maximum instance count reached!");
    _materials.push_back(new Material(this, _numInstances));
    Use();
    UPDATE_CALLINFO();
    for(auto ub : _uniformBlocks) ub.second->BindUniformBuffer(_numInstances, ID);
    UPDATE_CALLINFO();
    if(glGetUniformBlockIndex(ID, "GlobalUniforms") < 1000)
    UPDATE_CALLINFO();
        Renderer::GetInstance()->GetStdUniformStructs()["GlobalUniforms"]->BindUniformBuffer(0, ID);
    _numInstances++;
    return _materials.back();
}

void Shader::SetMat4(string name, glm::mat4 mat, uint32_t count)
{
    glUniformMatrix4fv(ULoc(name.c_str()), count, GL_FALSE, &mat[0][0]);
}

void Shader::SetInt(string name, int value)
{
    int location = ULoc( name.c_str());
    glUniform1i(location, value);
}

void Shader::SetFloat(string name, float value)
{
    int location = ULoc( name.c_str());
    glUniform1f(location, value);
}

void Shader::SetVec2(string name, glm::vec2 value)
{
    glUniform2fv(ULoc(name.c_str()), 1, &value[0]);
}

void Shader::SetVec3(string name, glm::vec3 value)
{
    glUniform3fv(ULoc(name.c_str()), 1, &value[0]);
}

void Shader::SetVec4(string name, glm::vec4 value)
{
    int location = ULoc( name.c_str());
    glUniform4fv(location, 1, &value[0]);
}

float Shader::GetFloat(string name)
{
    float v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v);
    return v;
}

glm::vec2 Shader::GetVec2(string name)
{
    glm::vec2 v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v[0]);
    return v;
}

glm::vec3 Shader::GetVec3(string name)
{
    glm::vec3 v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v[0]);
    return v;
}

glm::vec4 Shader::GetVec4(string name)
{
    glm::vec4 v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v[0]);
    return v;
}


Shader* Shader::WithStandardIncludes(std::string vertex, std::string fragment)
{
    string header = GetAbsoluteResourcesPath("\\shaders\\header.glsl");
    string structs = GetAbsoluteResourcesPath("\\shaders\\structs.glsl");
    string vstdinclude = GetAbsoluteResourcesPath("\\shaders\\std_include.vert");
    string fstdinclude = GetAbsoluteResourcesPath("\\shaders\\std_include.frag");
    return new Shader({header, structs, vstdinclude, vertex},{header, structs, fstdinclude, fragment});
}

Shader* Shader::GetWaveShader()
{
    auto shader = new Shader(GetAbsoluteResourcesPath("\\shaders\\wave_shader.vert"), GetAbsoluteResourcesPath("\\shaders\\wave_shader.frag"));
    return shader;
}

Shader* Shader::GetGerstnerWaveShader()
{
    string gerstner = GetAbsoluteResourcesPath("\\shaders\\gerstner_shader.vert");
    string fphong = GetAbsoluteResourcesPath("\\shaders\\phong_shader.frag");
    return WithStandardIncludes(gerstner, fphong);
}

Shader* Shader::GetGerstnerWaveShader_PBR()
{
    string gerstner = GetAbsoluteResourcesPath("\\shaders\\gerstner_shader.vert");
    string fgerstner = GetAbsoluteResourcesPath("\\shaders\\PBR_shader.frag");
    return WithStandardIncludes(gerstner, fgerstner);
}

Shader* Shader::GetPhongShader()
{
    string vphong = GetAbsoluteResourcesPath("\\shaders\\phong_shader.vert");
    string fphong = GetAbsoluteResourcesPath("\\shaders\\phong_shader.frag");
    return WithStandardIncludes(vphong, fphong);
}

Shader* Shader::GetPBRShader()
{
    string vpbr = GetAbsoluteResourcesPath("\\shaders\\PBR_shader.vert");
    string fpbr = GetAbsoluteResourcesPath("\\shaders\\PBR_shader.frag");
    return WithStandardIncludes(vpbr, fpbr);
}

Shader* Shader::GetTexturesPBRShader()
{
    string vpbr = GetAbsoluteResourcesPath("\\shaders\\PBR_shader.vert");
    string fpbr = GetAbsoluteResourcesPath("\\shaders\\PBR_shader_textures.frag");
    return WithStandardIncludes(vpbr, fpbr);
}


Shader* Shader::GetLineShader()
{
    return new Shader(GetAbsoluteResourcesPath("\\shaders\\line_shader.vert"), GetAbsoluteResourcesPath("\\shaders\\line_shader.frag"));
}

Shader* Shader::GetSkyboxShader()
{
    
    string vPath = GetAbsoluteResourcesPath("\\shaders\\Skybox.vert");
    string fPath = GetAbsoluteResourcesPath("\\shaders\\Skybox.frag");
    return WithStandardIncludes(vPath, fPath);
}

ShaderBuilder::ShaderBuilder(std::string name)
{
    _name = name;
    WithStandardHeader();
    WithStandardStructs();
    WithStandardIO();
    // WithStandardVertexFunctions();
}

ShaderBuilder& ShaderBuilder::WithStandardHeader()
{
    _header =   
        "#version 430 core\n"
        "#define MAX_LIGHTS 10\n"
        "#define PI 3.1415926535897932384626433832795\n";
    return *this;
}



ShaderBuilder& ShaderBuilder::WithStandardStructs()
{
    auto& structs = Renderer::GetInstance()->GetStdUniformStructs();
    GLSLStruct* plight = structs["PointLight"]->GetCopy();
    GLSLStruct* dlight = structs["DirectionalLight"]->GetCopy();
    GLSLStruct* camera = structs["Camera"]->GetCopy();
    GLSLStruct* props =  structs["StandardShadingProperties"]->GetCopy();
    GLSLStruct* pbr = structs["PBRProperties"]->GetCopy();
    GLSLStruct* instance = structs["InstanceUniforms"]->GetCopy();
    GLSLStruct* globals = structs["GlobalUniforms"];
    GLSLStruct* textures = structs["Textures"]->GetCopy();
    WithStruct(plight);
    WithStruct(dlight);
    WithStruct(camera);
    WithStruct(props);
    WithStruct(textures);
    WithUniformStruct(textures, "textures", false);
    WithUniformBlock(pbr, "PBR");
    WithUniformBlock(instance, "");
    WithUniformBlock(globals, "", true);
    // _uniformBlocks.push_back(pbr);
    // _uniformBlocks.push_back(instance);
    // _uniformBlocks.push_back(globals);
    _textures.push_back("textures.normal");
    _textures.push_back("textures.albedo");
    _textures.push_back("textures.roughness");
    _textures.push_back("textures.metallic");
    _textures.push_back("textures.ambient");

    return *this;
}

ShaderBuilder& ShaderBuilder::WithStandardIO()
{
    std::string vertex_io = "layout (location = 0) in vec3 v_position;\n"
                            "layout (location = 1) in vec3 v_normal;\n"
                            "layout (location = 2) in vec2 v_uv;\n"
                            "layout (location = 3) in vec3 v_tangent;\n"
                            "layout (location = 4) in vec3 v_bitangent;\n"
                            "out StandardShadingProperties Properties;\n";
    std::string fragment_io="layout (location = 0) out vec4 fragment_colour;\n"
                            "layout (location = 1) out vec4 bright_colour;\n"
                            "in StandardShadingProperties Properties;\n";
    _vertIO = vertex_io;
    _fragIO = fragment_io;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithStandardVertexFunctions()
{
    std::string calcTBNMat = 
        "void CalculateTBNMatrix(vec3 normal)\n"
        "{\n"
        "    vec3 N = normalize(vec3(ViewModel * vec4(normal, 0.0f)));\n"
        "    vec3 T = normalize(vec3(ViewModel * vec4(v_tangent, 0.0f)));\n"
        "    vec3 B = normalize(vec3(ViewModel * vec4(v_bitangent, 0.0f)));\n"
        "    // re-orthogonalize T with respect to N\n"
        "    T = normalize(T - dot(T, N) * N);\n"
        "    // then retrieve perpendicular vector B with the cross product of T and N\n"
        "    B = cross(N, T);\n"
        "    Properties.TBN = mat3(T,B,N);\n"
        "}\n";
    std::string calcProps = 
        "void CalculateStandardProperties()"
        "{\n"
        "    Properties.N = normalize(ViewModel * vec4(v_normal, 0.0f));      //Surface normal\n"
        "    Properties.ViewSpacePosition = ViewModel * vec4(v_position, 1.0f);\n"
        "    Properties.WorldSpacePosition = Model * vec4(v_position, 1.0f);\n"
        "    Properties.V = -normalize(Properties.ViewSpacePosition); //Surface to eye direction\n"
        "    Properties.L = -normalize(camera.View * vec4(directionalLight.Direction, 0.0f));      //Direction towards the light\n"
        "    if(dot(Properties.N,Properties.V) < 0) Properties.N = -Properties.N;\n"
        "    Properties.R = normalize(reflect(-Properties.L,Properties.N));\n"
        "    Properties.H = normalize(Properties.L+Properties.V); \n"
        "    Properties.UV = v_uv;\n"
        "    CalculateTBNMatrix(v_normal);\n"
        "};\n";
    std::string main = 
        "void main()\n"
        "{\n"
        "    CalculateStandardProperties();\n"
        // "    gl_Position = vec4(v_position, 1.0);\n"
        "    gl_Position = MVP * vec4(v_position, 1.0);\n"
        "};\n";
    _vertMain = calcTBNMat + calcProps + main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithWorldSpaceVertexFunctions()
{
    std::string calcTBNMat = 
        "void CalculateTBNMatrix(vec3 normal)\n"
        "{\n"
        "    vec3 N = normalize(vec3(Model * vec4(normal, 0.0f)));\n"
        "    vec3 T = normalize(vec3(Model * vec4(v_tangent, 0.0f)));\n"
        "    vec3 B = normalize(vec3(Model * vec4(v_bitangent, 0.0f)));\n"
        "    // re-orthogonalize T with respect to N\n"
        "    T = normalize(T - dot(T, N) * N);\n"
        "    // then retrieve perpendicular vector B with the cross product of T and N\n"
        "    B = cross(N, T);\n"
        "    Properties.TBN = mat3(T,B,N);\n"
        "}\n";
    std::string calcProps = 
        "void CalculateStandardProperties()"
        "{\n"
        "    Properties.N = normalize(Model * vec4(v_normal, 0.0f));      //Surface normal\n"
        "    Properties.ViewSpacePosition = ViewModel * vec4(v_position, 1.0f);\n"
        "    Properties.WorldSpacePosition = Model * vec4(v_position, 1.0f);\n"
        "    Properties.V = -normalize(Properties.ViewSpacePosition); //Surface to eye direction\n"
        "    Properties.L = -normalize(vec4(directionalLight.Direction, 0.0f));      //Direction towards the light\n"
        "    if(dot(Properties.N,Properties.V) < 0) Properties.N = -Properties.N;\n"
        "    Properties.R = normalize(reflect(-Properties.L,Properties.N));\n"
        "    Properties.H = normalize(Properties.L+Properties.V); \n"
        "    Properties.UV = v_uv;\n"
        "    CalculateTBNMatrix(v_normal);\n"
        "};\n";
    std::string main = 
        "void main()\n"
        "{\n"
        "    CalculateStandardProperties();\n"
        // "    gl_Position = vec4(v_position, 1.0);\n"
        "    gl_Position = MVP * vec4(v_position, 1.0);\n"
        "};\n";
    _vertMain = calcTBNMat + calcProps + main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithSphericalBillboarding()
{
    WithUniformStruct(GLSLStruct::Create("Billboard")
        .WithSampler2D("texture")
        .Build(), "billboard", true);
    WithUniformBlock(GLSLStruct::Create("Billboardprops")
        .WithVec2("size")
        .Build(true, 3), "bbProps");
    std::string main = 
        "void main()\n"
        "{\n"
        "    mat4 mv = ViewModel;\n"
        "    mv[0][0] = 1;\n"
        "    mv[0][1] = 0;\n"
        "    mv[0][2] = 0;\n"
        "    mv[1][0] = 0;\n"
        "    mv[1][1] = 1;\n"
        "    mv[1][2] = 0;\n"
        "    mv[2][0] = 0;\n"
        "    mv[2][1] = 0;\n"
        "    mv[2][2] = 1;\n"
        "    Properties.UV = v_uv;\n"
        "    vec3 pos = v_position;\n"
        "    pos.xy *=  bbProps.size;\n"
        "    gl_Position = (camera.Projection * mv) * vec4(pos, 1.0);\n"
        "    gl_Position /= gl_Position.w;\n"
        "    gl_Position.xy += pos.xy;\n"
        "};\n";

    _vertMain = main;
    return *this;
}


ShaderBuilder& ShaderBuilder::WithUnlitSurface()
{
    std::string main = 
        "void main()\n"
        "{\n"
        "   fragment_colour = texture(billboard.texture, Properties.UV);\n"
        "}\n";
    _fragMain = main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithSkyboxVertexFunctions()
{
    std::string main = 
    "out vec3 coordinates;\n"
    "void main()\n"
    "{\n"
    "   coordinates = v_position;\n"
    "   gl_Position = (camera.Projection * vec4(mat3(camera.View) * v_position, 1.0));\n"
    "}\n";
    _vertMain = main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithPBR()
{
    std::string f = Utilities::ReadFile(GetAbsoluteResourcesPath("\\shaders\\pbr\\frag_functions.frag"));
    _fragMain = f;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithPPVertexFunctions()
{
    std::string main = 
        "void main()\n"
        "{\n"
        "    Properties.UV = v_uv;\n"
        "    gl_Position = vec4(v_position, 1.0);\n"
        "};\n";
    _vertMain = main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithGBuffer()
{
    std::string io = 
        "in StandardShadingProperties Properties;\n"
        "layout (location = 0) out vec3 gPosition;\n"
        "layout (location = 1) out vec4 gNormal;\n"
        "layout (location = 2) out vec3 gReflectance;\n"
        "layout (location = 3) out vec4 gAlbedoSpec;\n"
        "layout (location = 4) out float gDepth;\n";
    std::string main =
        "vec4 CalculateNormalFromMap(vec2 uv)\n"
        "{\n"
        "    vec3 normal = texture(textures.normal, uv).xyz;\n"
        "    normal = normalize(normal * 2.0 - 1.0);\n"
        "    normal = normalize(Properties.TBN * normal);\n"
        "    return vec4(normal,0.0f);\n"
        "}\n\n"
        "void main()\n"
        "{\n"
        "   gPosition = Properties.WorldSpacePosition.rgb;\n"
        "   gNormal.rgb = CalculateNormalFromMap(Properties.UV).rgb;\n"
        "   gNormal.a = texture(textures.metallic, Properties.UV).r;\n"
        "   gReflectance = PBR.F0;\n"
        "   gAlbedoSpec.rgb = texture(textures.albedo, Properties.UV).rgb;\n"
        "   gAlbedoSpec.a = texture(textures.roughness, Properties.UV).r;\n"
        "   gDepth = gl_FragCoord.z;\n"
        "}\n";
    _fragIO = io;
    _fragMain = main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithDeferredPBRLighting()
{
    WithUniformStruct(GLSLStruct::Create("GBuffer")
        .WithSampler2D("position")
        .WithSampler2D("normal")
        .WithSampler2D("reflectance")
        .WithSampler2D("albedoSpec")
        .WithSampler2D("depth")
        .Build(), "gBuffer", true);
    
    std::string io = 
        "in StandardShadingProperties Properties;\n"
        "layout (location = 0) out vec4 lColour;\n";

    std::string f = Utilities::ReadFile(GetAbsoluteResourcesPath("\\shaders\\pbr\\deferred_light.frag"));
    _fragIO = io;
    _fragMain = f;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithSkybox(bool postDeferred)
{
    WithUniformStruct(GLSLStruct::Create("Skybox").WithSamplerCube("texture").Build(), "skybox", true);
    _textures.push_back("skybox.texture");
    std::string main;
    if(postDeferred)
    {
        WithUniformStruct(GLSLStruct::Create("GBuffer")
            .WithSampler2D("position")
            .WithSampler2D("normal")
            .WithSampler2D("reflectance")
            .WithSampler2D("albedoSpec")
            .WithSampler2D("depth")
            .Build(), "gBuffer", true);
        WithUniformStruct(GLSLStruct::Create("LBuffer")
            .WithSampler2D("colour")
            .Build(), "lBuffer", true);
        main = 
            "in vec3 coordinates;\n"
            "void main()\n"
            "{\n"
            "   float depth = 1.0f - texture(gBuffer.depth, gl_FragCoord.xy/viewportSize).x;\n"
            "   if(depth >= 0.99) fragment_colour = texture(skybox.texture, coordinates);\n"
            "   else fragment_colour = texture(lBuffer.colour, gl_FragCoord.xy/viewportSize);\n"
            "}\n";
    }
    else
    {
        main = 
            "in vec3 coordinates;\n"
            "void main()\n"
            "{\n"
            "   fragment_colour = texture(skybox.texture, coordinates);\n"
            "}\n";
    }

    _fragMain = main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithStruct(GLSLStruct* str)
{
    _vertBlocks += str->GetGLSLCode(false, false);
    _fragBlocks += str->GetGLSLCode(false, false);
    return *this;
}

ShaderBuilder& ShaderBuilder::WithUniformStruct(GLSLStruct* str, std::string varname, bool withDefinition)
{
    if(withDefinition)
    {
        _vertBlocks += str->GetGLSLCode(true, false, varname);
        _fragBlocks += str->GetGLSLCode(true, false, varname);
    }
    else
    {
        _vertBlocks += "uniform " + str->Name + " " + varname + ";\n";
        _fragBlocks += "uniform " + str->Name + " " + varname + ";\n";
    }
    return *this;
}

ShaderBuilder& ShaderBuilder::WithUniformBlock(GLSLStruct* str, std::string name, bool external)
{
    if(!external) _uniformBlocks.push_back(str);
    _vertBlocks += str->GetGLSLCode(true, true, name);
    _fragBlocks += str->GetGLSLCode(true, true, name);
    return *this;
}

// ShaderBuilder& ShaderBuilder::WithTexture(std::string name)
// {
//     return *this;
// }


Shader* ShaderBuilder::Build()
{
    std::string vFile = Utilities::GetAbsoluteResourcesPath("\\testout\\" + _name + ".vert");
    std::string fFile = Utilities::GetAbsoluteResourcesPath("\\testout\\" + _name + ".frag");
    std::string completeVert = _header + _vertBlocks + _vertIO + _vertMain;
    std::string completeFrag = _header + _fragBlocks + _fragIO + _fragMain;
    Utilities::SaveToTextFile(completeVert, vFile);
    Utilities::SaveToTextFile(completeFrag, fFile);
    Shader* shader = new Shader(_name, completeVert, completeFrag, _uniformBlocks, _textures);
    shader->_vFiles = { vFile };
    shader->_fFiles = { fFile };
    return shader;
}

ShaderBuilder Shader::Create(std::string name)
{
    return ShaderBuilder(name);
}


} // namespace Engine::Rendering
