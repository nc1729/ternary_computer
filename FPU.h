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
    // access to CPU flags
    Tryte _flags;

    // access to instruction and stack pointers
    Tryte _i_ptr;
    Tryte _s_ptr;

    /*
    input/output
    */
    // FREAD $X, Fy
    // Read float starting at address $X and store in Fy
    void read_float(TFloat& fy);
    // FWRITE Fx, $Y
    // Write contents of float Fx and store in addresses $Y, $Y+1, $Y+2
    void write_float(TFloat& fx);
    // FSHOW Fx
    // Send a float value to the console
    void show_float(TFloat& fx);
    // FTELL Fx
    // Fill a float from console input
    void tell_float(TFloat& fx);

    /*
    stack management
    */
    // FPEEK Fx
    // Copy top element on stack to register, without popping it off
    void peek_float(TFloat& fx);
    // FPUSH Fx
    // Push float Fx onto the stack and increment stack pointer accordingly
    void push_float(TFloat& fx);
    // FPOP Fx
    // Pop value from the stack into register Fx
    void pop_float(TFloat& fx);

    /*
    register handling
    */
    // FSET Fx, (Fy, $Y, d)
    // Set Fx equal to Fy or float at $Y, $Y+1, $Y+2, or double d
    void set_float(TFloat& fx, TFloat& fy);
    void set_float_to_addr(TFloat& fx);
    void set_float_to_num(TFloat& fx);
    // FSWAP Fx, Fy
    // Swap float registers Fx, Fy
    void swap_floats(TFloat& fx, TFloat& fy);

    /*
    arithmetic
    */
    // FADD Fx, (Fy, d)
    // Add Fx to Fy/d and store the result in Fx
    void add_floats(TFloat& fx, TFloat& fy);
    void add_num_to_float(TFloat& fx);
    // FMUL Fx, (Fy, d)
    // Multiply Fx to Fy/d and store the result in Fx
    void mult_floats(TFloat& fx, TFloat& fy);
    void mult_float_by_num(TFloat& fx);
    // FDIV Fx, (Fy, d)
    // Divide Fx by Fy/d and store the result in Fx
    void div_floats(TFloat& fx, TFloat& fy);
    void div_float_by_num(TFloat& fx);
    // FCMP Fx, (Fy, d)
    // Compare Fx to Fy/d
    // if Fx < Fy, set CPU compare flag to -
    // if Fx == Fy, set CPU compare flag to 0
    // if Fx > Fy, set CPU compare flag to +
    void compare_floats(TFloat& fx, TFloat& fy);
    void compare_float_to_num(TFloat& fx);
    // FFLIP Fx
    // Flip sign of Fx
    void flip_float(TFloat& fx);
    // FABS Fx
    // if negative, flip sign of X, else do nothing
    void abs_float(TFloat& fx);
    // HALT
    void halt_and_catch_fire();
    
    

public:
    // constructor
    FPU(Memory<19683>& memory, Console& console, Tryte& flags, Tryte& i_ptr, Tryte& s_ptr);
    // error flag
    bool error;
    // instruction handler
    void handle_instr(Tryte instruction);
    // reset - zeroes all registers, sets error flag to false, ready for new instruction
    void reset();
};