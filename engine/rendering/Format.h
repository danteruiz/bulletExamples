
#pragma once
#include <vector>
#include <cstdint>

enum Slots
{
    POSITION = 0,
    NORMAL,
    TEXCOORD
};

struct Attribute
{
    Attribute(unsigned int _slot, unsigned int _count, unsigned  int _stride, unsigned int _offset)
        : slot(_slot), count(_count), stride(_stride), offset(_offset) {}
    unsigned int slot { 0 };
    unsigned int count { 0 };
    unsigned int stride { 0 };
    unsigned int offset { 0 };
};

class Layout
{
public:
    void setAttribute(unsigned int slot, unsigned int count, unsigned int stride, unsigned int offset);
    void enableAttributes() const;
private:
    std::vector<Attribute> m_attributes;
};


enum Type : uint8_t
{
    Float = 0,
    Half,
    Int32,
    Int16,
    Int8,
    UInt32,
    UInt16,
    UInt8
};

enum Element : uint8_t
{
    RB = 0,
    RGB,
    RGBA,
    XY,
    XYZ,
    XYZW,
    Quat,
    UV
};

enum Dimension : uint8_t
{
    Scalar,
    Vec2,
    Vec3,
    Vec4
};

class Format
{
public:
    Format() : m_type(Type::Int32), m_element(Element::XYZ), m_dimension(Dimension::Scalar) {}
    Format(Type type, Element element, Dimension dimension) :
        m_type(type), m_element(element), m_dimension(dimension) {}

private:
    Type m_type;
    Element m_element;
    Dimension m_dimension;
};

