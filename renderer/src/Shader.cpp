#include "renderer/Shader.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/Debugging.hpp"
#include "utilities/Utilities.hpp"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>



using namespace std;
using namespace Utilities;

namespace Rendering
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
            string msg = string("ERROR::SHADER_COMPILATION_ERROR of type: ") + type + string("\n") + string(infoLog);
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
    WithStandardVertexFunctions();
}

ShaderBuilder& ShaderBuilder::WithStandardHeader()
{
    std::string header = "#version 430 core\n"
                         "#define MAX_LIGHTS 10\n"
                         "#define PI 3.1415926535897932384626433832795\n";
    _vert = header;
    _frag = header;
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
    WithUniformBlock(globals, "");
    _uniformBlocks.push_back(pbr);
    _uniformBlocks.push_back(instance);
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
    _vert += vertex_io;
    _frag += fragment_io;
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
    _vert += calcTBNMat + calcProps + main;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithPBR()
{
    std::string f = Utilities::ReadFile(GetAbsoluteResourcesPath("\\shaders\\pbr\\frag_functions.frag"));
    _frag += f;
    return *this;
}

ShaderBuilder& ShaderBuilder::WithStruct(GLSLStruct* str)
{
    _vert += str->GetGLSLCode(false, false);
    _frag += str->GetGLSLCode(false, false);
    return *this;
}

ShaderBuilder& ShaderBuilder::WithUniformStruct(GLSLStruct* str, std::string varname, bool withDefinition)
{
    if(withDefinition)
    {
        _vert += str->GetGLSLCode(true, false, varname);
        _frag += str->GetGLSLCode(true, false, varname);
    }
    else
    {
        _vert += "uniform " + str->Name + " " + varname + ";\n";
        _frag += "uniform " + str->Name + " " + varname + ";\n";
    }
    

    return *this;
}

ShaderBuilder& ShaderBuilder::WithUniformBlock(GLSLStruct* str, std::string name)
{
    _vert += str->GetGLSLCode(true, true, name);
    _frag += str->GetGLSLCode(true, true, name);
    return *this;
}

// ShaderBuilder& ShaderBuilder::WithTexture(std::string name)
// {
//     return *this;
// }


Shader* ShaderBuilder::Build()
{
    WithPBR();
    Utilities::SaveToTextFile(_vert, Utilities::GetAbsoluteResourcesPath("\\testout\\vertex.vert"));
    Utilities::SaveToTextFile(_frag, Utilities::GetAbsoluteResourcesPath("\\testout\\fragment.frag"));
    return new Shader(_name, _vert, _frag, _uniformBlocks, _textures);
}

ShaderBuilder Shader::Create(std::string name)
{
    return ShaderBuilder(name);
}


} // namespace Rendering
