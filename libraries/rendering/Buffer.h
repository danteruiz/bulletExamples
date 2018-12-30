#pragma once

class Buffer
{
public:
    Buffer() = default;
    ~Buffer() = default;
    unsigned int getID { return m_id; }
private:
    unsigned int m_id { 0 };
}
