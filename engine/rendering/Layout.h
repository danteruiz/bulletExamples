#pragma once

#include <cstdint>

#include "Format.h"

enum Slot : uint8_t
{
    POSITION = 0,
    NORMAL,
    TEXCOORD
};

struct Attribute
{
    Attribute(Slot slot, Format format, unsigned int offset)
        : m_slot(slot), m_format(format), m_offset(offset) {}
    Format m_format;
    Slot m_slot { 0 };
    size_t m_offset { 0 };
};

class Layout
{
public:
    Layout() = default;
    ~Layout() = default;

    void addAttribute(Attribute attribute);
    void addAttribute(Slot slot, Format format, size_t offset);
    void enableAttributes() const;
private:
    std::vector<Attribute> m_attributes;
};
