#pragma once

#include <string>
#include <memory>
#include <array>

#include "Format.h"

struct Texture
{
    std::string source;
    uint32_t id;
    uint32_t width;
    uint32_t height;

    Format format;
};



std::shared_ptr<Texture> createTextureFromGLTF(int width, int height, int component, int bits,
                                               unsigned char* data);

std::shared_ptr<Texture> loadTexture(std::string path);
std::shared_ptr<Texture> loadCubeMap(std::array<std::string, 6> imagePaths);
std::shared_ptr<Texture> loadIBLTexture(std::string path);
