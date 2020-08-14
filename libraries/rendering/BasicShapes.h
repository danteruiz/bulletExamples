#pragma once

#include <array>
#include <memory>

#include "Model.h"
class BasicShapes
{
public:
    enum ShapeTypes
    {
        CUBE = 0,
        SPHERE,
        TRIANGLE,
        NUM_SHAPES
    };

    BasicShapes();

    Model::Pointer getShape(int type);
private:
    std::array<Model::Pointer, NUM_SHAPES> m_shapes;
};
