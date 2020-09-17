#include "Texture.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include <gl/glew.h>

std::shared_ptr<Texture> loadTexture(std::string path)
{
    //path = "C:/Users/dante/code/hifi-content/avatars/GrayFoxWithEyes/GrayFoxWithEyeys/" + path;
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    return texture;
}


std::shared_ptr<Texture> createTextureFromGLTF(int width, int height, int component, int bits, unsigned char*  data)
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    glGenTextures(1, &texture->id);
    std::cout << "texture id: " << texture->id << std::endl;
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGBA;
    std::cout << "component: " << component << std::endl;
    if (component == 1) {
        format = GL_RED;
    } else if (component == 2) {
        format = GL_RG;
    } else if (component == 3) {
        format = GL_RGB;
    }

    GLenum type = GL_UNSIGNED_BYTE;
    if (bits == 8) {
        // ok
    } else if (bits == 16) {
        type = GL_UNSIGNED_SHORT;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 format, type, data);

    return texture;
}


std::shared_ptr<Texture> loadCubeMap(std::array<std::string, 6> imagePaths)
{
    auto texture = std::make_shared<Texture>();
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);
    int width, height, nrChannels;

    for (unsigned int i = 0; i < 6; ++i)
    {
        unsigned char* data = stbi_load(imagePaths[i].c_str(), &width, &height, &nrChannels, 0);


        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else
        {
            std::cout << "Failed to load: " << imagePaths[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    return texture;
}
