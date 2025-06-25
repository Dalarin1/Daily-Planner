#pragma once
#include "Primitives.hpp"
#include <ctime>

class Task
{
public:
    bool IsDone;
    const char *Text;
    Task(const char *_text = nullptr)
    {
        IsDone = false;
        Text = _text == nullptr ? "" : _text;
    }
    inline void Switch() noexcept(true) {
        IsDone = !IsDone;
    } 
};

class Calendar
{
};