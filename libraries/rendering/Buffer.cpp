#include "Buffer.h"


Buffer::Buffer(Buffer::Type type, size_t size, size_t count, void* data) : m_type(type), m_size(size), m_count(count)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(m_type, m_id);
    glBufferData(m_type, m_size, data, GL_DYNAMIC_DRAW);
};

void Buffer::bind() const
{
    glBindBuffer(m_type, m_id);
}

void Buffer::unbind() const
{
    glBindBuffer(m_type, 0);
}

void Buffer::setAttri(unsigned int slot, unsigned int size, unsigned int stride, unsigned int s)
{
    glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, stride, (void*)s);
    glEnableVertexAttribArray(slot);
}
