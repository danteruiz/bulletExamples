#pragma once

#include "Texture.h"
#include "Shader.h"
#include "Model.h"

#include <memory>

struct Skybox
{
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Model> model;
};
