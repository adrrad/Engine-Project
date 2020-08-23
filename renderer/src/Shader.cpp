#include "renderer/Shader.hpp"
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
    
Shader::Shader(string vertexPath, string fragmentPath)
{       
    _vFiles = {vertexPath};
    _fFiles = {fragmentPath};
    CompileShader();
}

Shader::Shader(vector<string> vertexPath, vector<string> fragmentPath)
{
    _vFiles = vertexPath;
    _fFiles = fragmentPath;
    CompileShader();

}

void Shader::CompileShader()
{
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
    for(uint32_t uniformIndex = 0; uniformIndex < count; uniformIndex++)
    {
        size_t bufferSize = 150;
        uint32_t nameLength = 0;
        int varSize = 0;
        GLenum type;
        char name[32];
        glGetActiveUniform(ID, uniformIndex, bufferSize, (GLsizei*)&nameLength, &varSize, &type, name);
        UniformData data = {type, name};
        switch(type)
        {
            case GL_FLOAT:
            data.f = GetFloat(data.Name);
            break;
            case GL_FLOAT_VEC2:
            data.f2 = GetVec2(data.Name);
            break;
            case GL_FLOAT_VEC3:
            data.f3 = GetVec3(data.Name);
            break;
            case GL_FLOAT_VEC4:
            data.f4 = GetVec4(data.Name);
            break;
            default:
            break;
        }
        _activeUniforms.push_back(data);
    }
}

bool Shader::RecompileShader()
{
    _vertexSource = "";
    _fragmentSource = "";
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

    int count = 0;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &count);
    for(uint32_t uniformIndex = 0; uniformIndex < count; uniformIndex++)
    {
        size_t bufferSize = 150;
        uint32_t nameLength = 0;
        int varSize = 0;
        GLenum type;
        char name[32];
        glGetActiveUniform(ID, uniformIndex, bufferSize, (GLsizei*)&nameLength, &varSize, &type, name);
        UniformData data = {type, name};
        switch(type)
        {
            case GL_FLOAT:
            data.f = GetFloat(data.Name);
            break;
            case GL_FLOAT_VEC2:
            data.f2 = GetVec2(data.Name);
            break;
            case GL_FLOAT_VEC3:
            data.f3 = GetVec3(data.Name);
            break;
            case GL_FLOAT_VEC4:
            data.f4 = GetVec4(data.Name);
            break;
            default:
            break;
        }
        _activeUniforms.push_back(data);
    }
    return true;
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
    _materials.push_back(new Material(this));
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


vector<UniformData> Shader::GetActiveUniforms()
{
    return _activeUniforms;
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

} // namespace Rendering
