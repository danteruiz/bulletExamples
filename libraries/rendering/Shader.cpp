#include "Shader.h"

#include "Shaders.h"

#include <fstream>
#include <sstream>

Shader::Shader(const std::string& fragmentSource, const std::string& vertexSource Source)
{
    std::string vertexCode;
    std::string fragmentCode;
   	// open up a stream so that the files 
    // can be reading to the a stringstream 
    std::ifstream vShaderFile(Vertex);
    std::ifstream fShaderFile(Fragment);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    //convert the stream into to string format
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    
}
