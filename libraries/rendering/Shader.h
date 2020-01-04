#pragma once

class Shader
{
public:
    Shader(const std::string& fragmentSource, const std::string& vertexSource);
    unsigned int getID() const { return m_id; }

private:
    unsigned int m_id;
};
