#include "renderer/Shader.hpp"

#include <glad/glad.h>

#include <fstream>
#include <sstream>

#include <iostream>
#include <filesystem>
using namespace std;

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
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, &mat[0][0]);
}

void Shader::SetFloat(std::string name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

} // namespace Rendering
