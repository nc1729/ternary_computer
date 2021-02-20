#pragma once
#include <array>
#include "Float.h"
#include "Memory.h"
#include "Console.h"

class FPU
{
private:
    // internal float registers
    TFloat _f1;
    TFloat _f2;
    TFloat _f3;
    TFloat _f4;
    TFloat _f5;
    TFloat _f6;
    TFloat _f7;
    TFloat _f8;
    TFloat _f9;
    std::array<TFloat*, 9> float_regs = {&_f1, &_f2, &_f3, &_f4, &_f5, &_f6, &_f7, &_f8, &_f9};

    // access to console
    Console _console;
    // access to main memory
    Memory<19683> _memory;

public:
    FPU(Memory<19683>& memory, Console& console);
    // add methods here
};