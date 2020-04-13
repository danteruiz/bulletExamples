#include "Buffer.h"


Buffer::Buffer() {};


bool Buffer::allocate()
{
    /*glGenBuffers(1, m_id);
n
    if (!handle)
    {
        return false;
    }
    */
    return true;
}


void Buffer::setSize(int count, size_t size)
{
    //m_count = count;
    //m_size = size;
}


void Buffer::setData(const void* data)
{
    // glBindBuffer(GL_ARRAY_BUFFER, m_id);
    //glBufferData
}
