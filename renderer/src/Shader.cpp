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
    
Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
        
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e) {
            throw std::exception("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckShaderStatus(vertex, "VERTEX");

        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
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

void Shader::CheckShaderStatus(uint32_t shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::string msg = std::string("ERROR::SHADER_COMPILATION_ERROR of type: ") + type + std::string("\n") + std::string(infoLog);
            cout << msg << endl;
            throw std::exception("ERROR::SHADER_COMPILATION_ERROR");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::string msg = std::string("ERROR::PROGRAM_LINKING_ERROR of type: ") + type + std::string("\n") + std::string(infoLog);
            cout << msg << endl;
            throw std::exception("ERROR::PROGRAM_LINKING_ERROR");
        }
    }
}


int Shader::ULoc(std::string name)
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

void Shader::SetMat4(std::string name, glm::mat4 mat, uint32_t count)
{
    glUniformMatrix4fv(ULoc(name.c_str()), count, GL_FALSE, &mat[0][0]);
}

void Shader::SetInt(std::string name, int value)
{
    int location = ULoc( name.c_str());
    glUniform1i(location, value);
}

void Shader::SetFloat(std::string name, float value)
{
    int location = ULoc( name.c_str());
    glUniform1f(location, value);
}

void Shader::SetVec2(std::string name, glm::vec2 value)
{
    glUniform2fv(ULoc(name.c_str()), 1, &value[0]);
}

void Shader::SetVec3(std::string name, glm::vec3 value)
{
    glUniform3fv(ULoc(name.c_str()), 1, &value[0]);
}

void Shader::SetVec4(std::string name, glm::vec4 value)
{
    int location = ULoc( name.c_str());
    glUniform4fv(location, 1, &value[0]);
}

float Shader::GetFloat(std::string name)
{
    float v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v);
    return v;
}

glm::vec2 Shader::GetVec2(std::string name)
{
    glm::vec2 v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v[0]);
    return v;
}

glm::vec3 Shader::GetVec3(std::string name)
{
    glm::vec3 v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v[0]);
    return v;
}

glm::vec4 Shader::GetVec4(std::string name)
{
    glm::vec4 v;
    glGetUniformfv(ID, ULoc(name.c_str()), &v[0]);
    return v;
}


vector<UniformData> Shader::GetActiveUniforms()
{
    return _activeUniforms;
}


Shader* Shader::GetWaveShader()
{
    auto shader = new Shader(GetAbsoluteResourcesPath("\\shaders\\wave_shader.vert"), GetAbsoluteResourcesPath("\\shaders\\wave_shader.frag"));
    return shader;
}

Shader* Shader::GetGerstnerWaveShader()
{
    return new Shader(GetAbsoluteResourcesPath("\\shaders\\gerstner_shader.vert"), GetAbsoluteResourcesPath("\\shaders\\wave_shader.frag"));
}

Shader* Shader::GetPhongShader()
{
    return new Shader(GetAbsoluteResourcesPath("\\shaders\\phong_shader.vert"), GetAbsoluteResourcesPath("\\shaders\\phong_shader.frag"));
}

Shader* Shader::GetLineShader()
{
    return new Shader(GetAbsoluteResourcesPath("\\shaders\\line_shader.vert"), GetAbsoluteResourcesPath("\\shaders\\line_shader.frag"));
}

} // namespace Rendering
