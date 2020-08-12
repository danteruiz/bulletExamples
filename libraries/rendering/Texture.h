#pragma once

#include <string>
struct Texture
{
    std::string source;
    unsigned int id;
    unsigned int width;
    unsigned int height;
    unsigned char* data { nullptr };
};




unsigned int loadTexture(std::string file);
struct TextureHandle {
    unsigned int id;
};
