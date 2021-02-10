#pragma once

#if OS_LINUX
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif
#include <memory>

class Layout;
class Buffer
{
public:
    using Pointer = std::shared_ptr<Buffer>;
    enum Type
    {
        ARRAY = GL_ARRAY_BUFFER,
        ELEMENT = GL_ELEMENT_ARRAY_BUFFER
    };

    Buffer(Buffer::Type type, size_t size, size_t count, void *data);

    void bind() const;
    void unbind() const;
    void setLayout(std::shared_ptr<Layout> layout);
    std::shared_ptr<Layout>  getLayout() const;
    unsigned int getID() { return m_id; }
private:
    unsigned int m_id { 0 };
    size_t m_size { 0 };
    size_t m_count { 0 };
    Buffer::Type m_type;
    std::shared_ptr<Layout> m_layout;
};
