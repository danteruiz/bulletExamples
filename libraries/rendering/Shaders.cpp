#include "Shaders.h"

#include <iostream>

namespace shader {
    bool compileShader(GLenum shaderType, const std::string& shaderSource, GLuint& programObject, std::string& message)
    {
        programObject = glCreateShader(shaderType);

        if (!programObject) {
            std::cout << "Failed to create shader program \n";
            return false
        }
        glShaderSource(shaderID, 1, &(shaderSource.c_str()), NULL);
        glCompileShader(shaderID);

        GLuint compiled = 0;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
        getShaderInfoLog(shaderID, message);

        if (!compiled) {
            return false;
        }

        return true;
    }

    GLuint buildProgram(std::vector<GLuint>& shaders)
    {
        GLuint glprogram = glCreateProgram();

        for (auto shader: shaders) {
            glAttachShader(glprogram, shader);
        }

        return glprogram;
    }

    bool linkProgram(GLuint shaderProgram, std::string& message)
    {
        glLinkProgram(shaderProgram);

        GLuint linked = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);

        if (!linked) {
            std::cout << "shader not linked \n";
            return false;
        }
        return true;
    }
}
