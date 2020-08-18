#include "Format.h"

#include <assert.h>

#include <gl/glew.h>
void Layout::setAttribute(unsigned int slot, unsigned int count, unsigned int stride, unsigned int offset)
{
    m_attributes.push_back(Attribute(slot, count, stride, offset));
}

void Layout::enableAttributes() const
{
    assert(m_attributes.size() > 0);
    for (const auto& attribute : m_attributes)
    {
        glEnableVertexAttribArray(attribute.slot);
        glVertexAttribPointer(attribute.slot, attribute.count,
                              GL_FLOAT, GL_FALSE, attribute.stride,
                              (void*) attribute.offset);
    }
}
