#pragma once
#include "Primitives.h"
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