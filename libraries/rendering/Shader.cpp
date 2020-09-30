#include "Shader.h"

#include "Shaders.h"

#include <fstream>
#include <sstream>
#include <iostream>


std::string getSourceCode(std::string const &filePath, std::string const &defines)
{
    std::string sourceCode;

    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    sourceCode = shaderStream.str();
    sourceCode.insert(17, "\n" + defines);
    return sourceCode;
}

Shader::Shader(std::string const &fragmentSource, std::string const &vertexSource, std::string const &defines)
{
    std::string vertexCode = getSourceCode(vertexSource, defines);
    std::string fragmentCode = getSourceCode(fragmentSource, defines);
    std::string message;

    GLuint vertexShader, fragmentShader;

    shader::compileShader(GL_VERTEX_SHADER, vertexCode, vertexShader, message);
    shader::compileShader(GL_FRAGMENT_SHADER, fragmentCode, fragmentShader, message);
    m_id = shader::buildProgram({ vertexShader, fragmentShader});
    shader::linkProgram(m_id, message);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void Shader::bind() const
{
    glUseProgram(m_id);
}

void Shader::setUniformMat4(std::string const &name, glm::mat4 const &matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::setUniformVec4(std::string const &name, glm::vec4 const &vec) const
{
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setUniformVec3(std::string const &name, glm::vec3 const &vec) const
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setUniform1f(std::string const &name, float f) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), f);
}

void Shader::setUniform1i(std::string const &name, int i) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), i);
}
