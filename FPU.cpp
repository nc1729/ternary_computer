#include "FPU.h"
#include "Float.h"
#include "Console.h"
#include "Memory.h"

FPU::FPU(Memory<19683>& memory, Console& console)
{
    // FPU has access to memory and console
    _memory = memory;
    _console = console;

    // zero all registers
    for (auto& reg : float_regs)
    {
        *reg = 0.0;
    }
}