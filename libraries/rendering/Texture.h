#pragma once

struct Texture
{
    std::string source;
    unsigned int id;
    unsigned int width;
    unsigned int height;
    unsigned char* data { nullptr };
};
