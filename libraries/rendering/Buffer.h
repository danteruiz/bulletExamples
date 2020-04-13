#pragma once


#include <gl/glew.h>
class Buffer
{
public:
    Buffer();
    ~Buffer() = default;

    bool allocate();
    void setData(const void* data);
    void setSize(int count, size_t size);
    void setAttri(unsigned int slot, unsigned int size, unsigned int stride, unsigned int s);
    unsigned int getID() { return m_id; }
private:
    unsigned int m_id { 0 };
};
