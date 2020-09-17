#pragma once

#include <string>
#include <memory>
#include <array>



struct Texture
{
    std::string source;
    unsigned int id;
    unsigned int width;
    unsigned int height;
    unsigned char* data { nullptr };
};



std::shared_ptr<Texture> createTextureFromGLTF(int width, int height, int component, int bits, unsigned char* data);
std::shared_ptr<Texture> loadTexture(std::string path);
std::shared_ptr<Texture> loadCubeMap(std::array<std::string, 6> imagePaths);
