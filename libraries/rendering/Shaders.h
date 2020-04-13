#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
namespace shader {
    bool compileShader(GLenum shaderType, const std::string& shaderSource, GLuint& programObject, std::string& message);
    GLuint buildProgram(const std::vector<GLuint>& shaders);
    bool linkProgram(GLuint shaderProgram, std::string& message);
}
