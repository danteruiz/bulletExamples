#include "Layout.h"


void Layout::addAttribute(Attribute attribute)
{
    m_attributes.emplace_back(attribute);
}

void Layout::addAttribute(Slot slot, Format format, size_t offset)
{
    m_attributes.emplace_back(Attribute(slot, format, offset));
}
