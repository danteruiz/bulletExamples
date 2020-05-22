#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Shader
{
public:
    Shader(std::string const &fragmentSource, std::string const &vertexSource);
    int getID() const { return m_id; }

    void bind() const;
    void setUniformMat4(std::string const &name, glm::mat4 const &matrix) const;
    void setUniformVec4(std::string const &name, glm::vec4 const &vec) const;
    void setUniformVec3(std::string const &name, glm::vec3 const &vec) const;
    void setUniform1f(std::string const &name, float f) const;

private:
    int m_id;
};
