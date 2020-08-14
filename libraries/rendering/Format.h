#pragma once


enum Slots
{
    POSITION = 0,
    NORMAL,
    TEXTCOORD
};

struct Attriute
{
    unsigned int slot;
    unsigned int count;
    unsigned int stride;
    unsigned int offset;
};

class Layout
{
public:
    void setAttribute(unsigned int slot, unsigned int count, unsigned int stride
};

