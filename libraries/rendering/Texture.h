#pragma once

#include <string>
#include <memory>

struct Texture
{
    std::string source;
    unsigned int id;
    unsigned int width;
    unsigned int height;
    unsigned char* data { nullptr };
};

std::shared_ptr<Texture> loadTexture(std::string file);
