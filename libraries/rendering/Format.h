#pragma once
#include <vector>

enum Slots
{
    POSITION = 0,
    NORMAL,
    TEXTCOORD
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

