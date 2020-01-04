#pragma once

#include <string>
namespace shader {

    bool compileShader(const std::string& shaderSource, GLuint programObject, std::string& message);
    GLuint buildProgram(const std::vector<GLuint>& shaders);
    bool linkProgram(GLuint shaderProgram, std::string& message);
}
