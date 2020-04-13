#include "Shader.h"

#include "Shaders.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& fragmentSource, const std::string& vertexSource)
{
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile(vertexSource);
    std::ifstream fShaderFile(fragmentSource);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    std::string message;

    GLuint vertexShader, fragmentShader;
    shader::compileShader(GL_VERTEX_SHADER, vertexCode, vertexShader, message);
    shader::compileShader(GL_FRAGMENT_SHADER, fragmentCode, fragmentShader, message);
    m_id = shader::buildProgram({ vertexShader, fragmentShader});
    shader::linkProgram(m_id, message);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
