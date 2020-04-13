#pragma once

#include <string>
class Shader
{
public:
    Shader(const std::string& fragmentSource, const std::string& vertexSource);
    int getID() const { return m_id; }

private:
    int m_id;
};
