#include <array>
#include "FPU.h"
#include "Float.h"
#include "Console.h"
#include "Memory.h"

FPU::FPU(Memory<19683>& memory, Console& console, Tryte& flags, Tryte& i_ptr, Tryte& s_ptr)
{
    // FPU has access to memory and console, and CPU flags
    _memory = memory;
    _console = console;
    _flags = flags;

    // FPU has access to instruction and stack pointers
    _i_ptr = i_ptr;
    _s_ptr = s_ptr;

    // zero all registers
    for (auto& reg : float_regs)
    {
        *reg = 0.0;
    }

    // set error to false - if halt_and_catch_fire triggered, FPU will signal an error
    // CPU should check for this and trigger a halt
    error = false;
}
void FPU::handle_instr(Tryte instruction)
{
    // read instruction is 'fXY' - second = X, third = Y
    std::array<int16_t, 9> tern_array = Tryte::ternary_array(instruction);
    int16_t high_3 = 9 * tern_array[0] + 3 * tern_array[1] + tern_array[2];
    int16_t mid_3 = 9 * tern_array[3] + 3 * tern_array[4] + tern_array[5];
	int16_t high_2 = 3 * tern_array[3] + tern_array[4] + 4;
	int16_t mid_2 = 3 * tern_array[5] + tern_array[6] + 4;
	int16_t low_2 = 3 * tern_array[7] + tern_array[8] + 4;
    if (high_3 == 6)
    {
        switch (high_2)
        {
            case 0:
                // f(M-K)(M-m) - FSET Fx, Fy
                set_float(*float_regs[mid_2], *float_regs[low_2]);
                break;
            
            case 1:
                // f(J-H)(M-m) - FCMP Fx, Fy
                compare_floats(*float_regs[mid_2], *float_regs[low_2]);
                break;
            
            case 2:
                // f(G-E)(M-m) - FADD Fx, Fy
                add_floats(*float_regs[mid_2], *float_regs[low_2]);
                break;
            
            case 3:
                // f(D-B)(M-m) - FMUL Fx, Fy
                mult_floats(*float_regs[mid_2], *float_regs[low_2]);
                break;
            
            case 4:
                // f(A-a)(M-m) - FDIV Fx, Fy
                div_floats(*float_regs[mid_2], *float_regs[low_2]);
                break;
            
            case 8:
                // f(k-m)(M-m) - FSWAP Fx, Fy
                swap_floats(*float_regs[mid_2], *float_regs[low_2]);
                break;
            
            default:
                halt_and_catch_fire();
                break;
        }
    }
    else if (high_3 == 7)
    {
        switch (mid_3)
        {
            case 0:
                // g0X - FCMP X, n
                compare_float_to_num(*float_regs[low_2]);
                break;
            case 1:
                // gaX - FSHOW Fx
                show_float(*float_regs[low_2]);
                break;
            case -1:
                // gAX - FTELL Fx
                tell_float(*float_regs[low_2]);
                break;
            case 2:
                // gbX - FPUSH Fx
                push_float(*float_regs[low_2]);
                break;
            case -2:
                // gBX - FPOP Fx
                pop_float(*float_regs[low_2]);
                break;
            case 3:
                // gcX - FSET Fx, $Y
                set_float_to_addr(*float_regs[low_2]);
                break;
            case -3:
                // gCX - FSET Fx, n
                set_float_to_num(*float_regs[low_2]);
                break;
            case 4:
                // gdX - FFLIP Fx
                flip_float(*float_regs[low_2]);
                break;
            case -4:
                // gDX - FABS Fx
                abs_float(*float_regs[low_2]);
                break;
            case 5:
                // geX - FADD Fx, n
                add_num_to_float(*float_regs[low_2]);
                break;
            case 6:
                // gfX - FMUL Fx, n
                mult_float_by_num(*float_regs[low_2]);
                break;
            case 7:
                // ggX - FDIV Fx, n
                div_float_by_num(*float_regs[low_2]);
                break;
            case 12:
                // glX - PEEK Fx
                peek_float(*float_regs[low_2]);
                break;
            case 13:
                // gmY - FREAD $X, Fy
                read_float(*float_regs[low_2]);
                break;
            case -13:
                // gMY - FWRITE Fx, $Y
                write_float(*float_regs[low_2]);
                break;
            default:
                halt_and_catch_fire();
        }
    }
    else
    {
        halt_and_catch_fire();
    }      
}
void FPU::reset()
{
    // zero all registers
    for (auto& reg : float_regs)
    {
        *reg = 0.0;
    }

    error = false;
}

/*
input/output
*/
void FPU::read_float(TFloat& fy)
{
    Tryte& add_x = _memory[_i_ptr + 1];
    Trint<1> exponent = Trint<1>(std::array<Tryte, 1>({_memory[add_x]}));
    Trint<2> mantissa = Trint<2>(std::array<Tryte, 2>({_memory[add_x + 1], _memory[add_x + 2]}));
    fy = TFloat(exponent, mantissa);
    _i_ptr += 2;
}
void FPU::write_float(TFloat& fx)
{
    Tryte& add_x = _memory[_i_ptr + 1];
    _memory[add_x] = TFloat::get_exponent(fx)[0];
    Trint<2> fx_mantissa = TFloat::get_mantissa(fx);
    _memory[add_x + 1] = fx_mantissa[0];
    _memory[add_x + 2] = fx_mantissa[1];
    _i_ptr += 2;
}
void FPU::show_float(TFloat& fx)
{
    _console << fx;
    _i_ptr += 1;
}
void FPU::tell_float(TFloat& fx)
{
    char c[2] = {0, 0};
    // first fetch exponent tryte
    _console >> c[0] >> c[1];
    int16_t exponent_tryte_value = 128 * static_cast<int16_t>(c[0]) + static_cast<int16_t>(c[1]) - 9841;
    Trint<1> fx_exponent = Trint<1>(Tryte(exponent_tryte_value));
    // then fetch mantissa trytes
    _console >> c[0] >> c[1];
    int16_t mantissa_tryte_value1 = 128 * static_cast<int16_t>(c[0]) + static_cast<int16_t>(c[1]) - 9841;
    _console >> c[0] >> c[1];
    int16_t mantissa_tryte_value2 = 128 * static_cast<int16_t>(c[0]) + static_cast<int16_t>(c[1]) - 9841;
    Trint<2> fx_mantissa = Trint<2>(std::array<Tryte, 2>({Tryte(mantissa_tryte_value1), Tryte(mantissa_tryte_value2)}));
    fx = TFloat(fx_exponent, fx_mantissa);
    _i_ptr += 1;
}

/*
stack management
*/
void FPU::peek_float(TFloat& fx)
{
    std::array<Tryte, 3> stack_trytes = { _memory[_s_ptr - 1], _memory[_s_ptr - 2], _memory[_s_ptr - 3] };
	fx = TFloat(stack_trytes[0], stack_trytes[1], stack_trytes[2]);
	_i_ptr += 1;
}
void FPU::push_float(TFloat& fx)
{
    Trint<1> fx_exponent = TFloat::get_exponent(fx);
    Trint<2> fx_mantissa = TFloat::get_mantissa(fx);
    _memory[_s_ptr] = fx_exponent[0];
    _memory[_s_ptr + 1] = fx_mantissa[0];
    _memory[_s_ptr + 2] = fx_mantissa[1];
    _s_ptr += 3;
    _i_ptr += 1;
}
void FPU::pop_float(TFloat& fx)
{
    std::array<Tryte, 3> stack_trytes = { _memory[_s_ptr - 1], _memory[_s_ptr - 2], _memory[_s_ptr - 3] };
	fx = TFloat(stack_trytes[0], stack_trytes[1], stack_trytes[2]);
    _s_ptr -= 3;
	_i_ptr += 1;
}

/*
register handling
*/
void FPU::set_float(TFloat& fx, TFloat& fy)
{
    fx = fy;
    _i_ptr += 1;
}
void FPU::set_float_to_addr(TFloat& fx)
{
    Tryte& add_x = _memory[_i_ptr + 1];
    TFloat new_float(_memory[add_x], _memory[add_x + 1], _memory[add_x + 2]);
    fx = new_float;
    _i_ptr += 2;
}
void FPU::set_float_to_num(TFloat& fx)
{
    TFloat new_float(_memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3]);
    fx = new_float;
    _i_ptr += 4;
}
void FPU::swap_floats(TFloat& fx, TFloat& fy)
{
    TFloat temp = fx;
    fx = fy;
    fy = temp;
    _i_ptr += 1;
}

/*
arithmetic
*/
void FPU::add_floats(TFloat& fx, TFloat& fy)
{
    fx += fy;
    _i_ptr += 1;
}
void FPU::add_num_to_float(TFloat& fx)
{
    TFloat num(_memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3]);
    fx += num;
    _i_ptr += 4;
}
void FPU::mult_floats(TFloat& fx, TFloat& fy)
{
    fx *= fy;
    _i_ptr += 1;
}
void FPU::mult_float_by_num(TFloat& fx)
{
    TFloat num(_memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3]);
    fx *= num;
    _i_ptr += 4;
}
void FPU::div_floats(TFloat& fx, TFloat& fy)
{
    fx /= fy;
    _i_ptr += 1;
}
void FPU::div_float_by_num(TFloat& fx)
{
    TFloat num(_memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3]);
    fx /= num;
    _i_ptr += 4;
}
void FPU::compare_floats(TFloat& fx, TFloat& fy)
{
    if (fx < fy)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) - 1;
	}
	else if (fx > fy)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) + 1;
	}
	else
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0"));
	}
	_i_ptr += 1;
}
void FPU::compare_float_to_num(TFloat& fx)
{
    TFloat num(_memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3]);
    if (fx < num)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) - 1;
	}
	else if (fx > num)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) + 1;
	}
	else
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0"));
	}
	_i_ptr += 4;
}
void FPU::flip_float(TFloat& fx)
{
    fx = -fx;
    _i_ptr += 1;
}
void FPU::abs_float(TFloat& fx)
{
    fx = TFloat::abs(fx);
    _i_ptr += 1;
}
void FPU::halt_and_catch_fire()
{
    error = true;
    _i_ptr += 1;
}