#include "Buffer.h"

#include "Layout.h"


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

void Buffer::applyLayout() const
{
    for (auto element : m_layout) {
    }
}

void Buffer::setLayout(std::shared_ptr<Layout> layout) { m_layout = layout; }

std::shared_ptr<Layout> Buffer::getLayout() const { return m_layout; }
