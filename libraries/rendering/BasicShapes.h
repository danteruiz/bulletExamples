#pragma once

#include <array>
#include <memory>


struct Geometry;
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

    std::shared_ptr<Geometry> getShape(int type);
private:
    std::array<std::shared_ptr<Geometry>, NUM_SHAPES> m_shapes;
};
