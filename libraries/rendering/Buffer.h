#pragma once


#include <gl/glew.h>
class Buffer
{
public:

    enum Type
    {
        ARRAY = GL_ARRAY_BUFFER,
        ELEMENT = GL_ELEMENT_ARRAY_BUFFER
    };

    Buffer(Buffer::Type type, size_t size, size_t count, void *data);


    void bind() const;
    void unbind() const;
    void setAttri(unsigned int slot, unsigned int size, unsigned int stride, unsigned int s = 0);
    unsigned int getID() { return m_id; }
private:
    unsigned int m_id { 0 };
    size_t m_size { 0 };
    size_t m_count { 0 };
    Buffer::Type m_type;
};
