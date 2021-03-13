#pragma once

#include <memory>

#inclue "Format.h"

class Buffer
{
public:
    using Pointer = std::shared_ptr<Buffer>;
    Buffer();
    size_t getNumElements() const { return m_numElements; }
    void* data() const { return m_data; }
    bool setData(void const *data
private:
    size_t m_size;
    void* m_data { nullptr };
};
