#pragma once

#include <string>
#include <chrono>
#include <iostream>

template <typename T> class StopWatch : T
{
    typedef typename T BaseTimer;
public:
    explicit StopWatch(std::string const &category) : m_category(category)
    {
        start();
    }

    ~StopWatch()
    {
        int64_t duration = getMs();
        std::cout << m_category << " : " << duration << std::endl;
    }

private:
    std::string m_category;
};

class ChronoTimerBase
{
public:
    ChronoTimerBase() :  m_start(std::chrono::high_resolution_clock::now()) { }
    void start()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    int64_t getMs()
    {
        auto diff = std::chrono::high_resolution_clock::now() - m_start;
        return (int64_t) (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;
};

using ChronoStopWatch = StopWatch<ChronoTimerBase>;


