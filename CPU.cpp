#include "CPU.h"
#include "Console.h"
#include "FPU.h"
#include <vector>
#include <string>
#include <array>
#include <fstream>
#include <stdexcept>

CPU::CPU(Memory<19683>& memory, std::vector<std::string>& disknames)
{
	_memory = memory;
	_disknames = disknames;
	_console = Console();
	_clock = 0;
	_on = false;
	// zero all registers
	for (auto& reg : trint_regs)
	{
		*reg = 0;
	}
	_i_ptr = Tryte(0);
	_s_ptr = Tryte("MMM");
	// set up interrupt array - on boot, all interrupts point to 000
	for (auto& int_ptr : _int_ptrs)
	{
		int_ptr = Tryte(0);
	}

	// set disk to 0
	_disk_num = 0;

	// set instruction to 0
	_instr = Tryte(0);

	// initialise flags - stored interrupt priority is -13, current thread has priority 0.
	// overflow, carry and compare flags set to 0.
	_flags = Tryte("M00");
}

void CPU::fetch()
{
	_instr = _memory[_i_ptr];
}

void CPU::decode_and_execute()
{
	std::string instruction = Tryte::septavingt_string(_instr);
	char first = instruction[0];
	char second = instruction[1];
	char third = instruction[2];
	std::array<int16_t, 9> tern_array = Tryte::ternary_array(_instr);
	int16_t high_2 = 3 * tern_array[3] + tern_array[4] + 4;
	int16_t mid_2 = 3 * tern_array[5] + tern_array[6] + 4;
	int16_t low_2 = 3 * tern_array[7] + tern_array[8] + 4;
	int16_t low_3 = 9 * tern_array[6] + 3 * tern_array[7] + tern_array[8];
	switch (first)
	{
		case '0':
		// 0XX - control flow
		switch (second)
		{
			case '0':
			// 00X - stopping/waiting
			switch (third)
			{
				case '0':
					// 000 - HALT
					halt_and_catch_fire();
					break;
				case 'a':
					// 00a - NOOP
					noop();
					break;
				case 'A':
					// 00A - WAIT
					wait();
					break;
				
				default:
					halt_and_catch_fire();
					break;
			}
			break;

			case 'a':
			// 0aX - compare flag management
			switch (third)
			{
				case '0':
					// 0a0 - CCMP
					clear_compare();
					break;
				case 'A':
					// 0aA - CCAR
					clear_carry();
					break;
				case 'a':
					// 0aa - COVF
					clear_overflow();
					break;
				
				default:
				halt_and_catch_fire();
				break;
			}
			break;

			case 'b':
				// 0b - PJP
				pop_and_jump();
				break;

			case 'B':
				// 0B - jump and store
				jump_and_store();
				break;

			case 'c':
				// 0c - CHK
				check_priority();
				break;

			case 'h':
				// 0hn - THD n
				switch_thread(low_3);
				break;

			case 'I':
				// 0In - PRI n
				set_priority(low_3);
				break;

			case 'i':
				// 0in - INT $x, n
				set_interrupt_ptr(low_3);
				break;

			case 'j':
			    // 0jX - jump instructions
				switch (third)
				{
					case '0':
						// 0j0 - JPZ $X
						jump_if_zero();
						break;
						
					case 'a':
						// 0ja - JPP $X
						jump_if_pos();
						break;

					case 'A':
					    // 0jA - JPN $X
						jump_if_neg();
						break;

					case 'm':
						// 0jm - JPS $X
						jump_and_store();
						break;

					case 'M':
						// 0jM - PJP
						pop_and_jump();
						break;

					case 'j':
						// 0jj - JP $X
						jump();
						break;

					default:
						halt_and_catch_fire();
						break;
				}
				break;

			case 'm':
				// 0m - MOUNT n
				mount(low_3 + 13);
				break;

			default:
				halt_and_catch_fire();
				break;
		}
	    break;

		case 'a':
			// aXX - input/output
			switch (second)
			{
				case 'A':
					// aAY - READ $X, Y
					read_tryte(*tryte_regs[third]);
					break;

				case 'a':
					// aaY - READ $X, Y
					read_trint(*trint_regs[low_2]);
					break;

				case 'B':
					// aBX - WRITE X, $Y
					write_tryte(*tryte_regs[third]);
					break;

				case 'b':
					// abX - WRITE X, $Y
					write_trint(*trint_regs[low_2]);
					break;

				case 'f':
					// af - FILL $X, N, K
					fill();
					break;

				case 'M':
					// aM - LOAD $X, N, $Y
					load();
					break;

				case 'm':
					// am - SAVE $X, N, $Y
					save();
					break;

				default:
					halt_and_catch_fire();
					break;
			}
			break;

		case 'b':
			// bXX - stack management
			switch (second)
			{
				case '0':
					// b0X - WHERE X
					where(*tryte_regs[third]);
					break;

				case 'A':
					// bAX - PUSH X
					push_tryte(*tryte_regs[third]);
					break;

				case 'a':
					// baX - PUSH X
					push_trint(*trint_regs[low_2]);
					break;

				case 'B':
					// bBX - POP X
					pop_tryte(*tryte_regs[third]);
					break;

				case 'b':
					// bbX - POP X
					pop_trint(*trint_regs[low_2]);
					break;

				case 'M':
					// bMX - PEEK X
					peek_tryte(*tryte_regs[third]);
					break;

				case 'm':
					// bmX - PEEK X
					peek_trint(*trint_regs[low_2]);
					break;

				default:
					halt_and_catch_fire();
					break;
			}
			break;
		
		case 'c':
			// cXX - console management
			switch (second)
			{
				case '0':
					// c0 - PRINT $X, n
					print();
					break;
				case 'a':
					// caX - DSET X
					set_display_mode(*trint_regs[low_2]);
					break;
				case 'A':
					// cAX - DSET X
					set_display_mode(*tryte_regs[third]);
					break;
				case 'b':
					// cbX - DGET X
					get_display_mode(*trint_regs[low_2]);
					break;
				case 'B':
					// cBX - DGET X
					get_display_mode(*tryte_regs[third]);
					break;
				case 'c':
					// ccX - SHOW X
					show_trint(*trint_regs[low_2]);
					break;
				case 'C':
					// cCX - SHOW X
					show_tryte(*tryte_regs[third]);
					break;
				case 'd':
					// cdX - TELL X
					tell_trint(*trint_regs[low_2]);
					break;
				case 'D':
					// cDX - TELL X
					tell_tryte(*tryte_regs[third]);
					break;
				case 'm':
					// cmn - DSET n
					set_display_mode(low_3 + 13);
					break; 
			}
			break; 

		case 'A':
			// AXY - add trytes
			// ADD X, Y
			add_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;
		
		case 'B':
			// BXY - set tryte to tryte
			// SET X, Y
			set_tryte(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'C':
			// CXY - compare tryte to tryte
			// CMP X, Y
			compare_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'D':
			// DXY - divide tryte by tryte
			// DIV X, Y
			div_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'E':
			// EXY - multiply tryte by tryte
			// MUL X, Y
			mult_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'f':
			// fXY - floating point operations
			// pass instruction to FPU - FPU will decode and execute the instruction
			_FPU.handle_instr(instruction);
			if (_FPU.error)
			{
				halt_and_catch_fire();
			}
			break;
		
		case 'g':
			// gXY - floating point operations
			// pass instruction to FPU - FPU will decode and execute the instruction
			_FPU.handle_instr(instruction);
			if (_FPU.error)
			{
				halt_and_catch_fire();
			}
			break;

		case 'F':
			// FXY - AND trytes
			// AND X, Y
			and_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'G':
			// GXY - OR trytes
			// OR X, Y
			or_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'H':
			// HXY - XOR trytes
			// XOR X, Y
			xor_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'I':
			// IXY - swap trytes
			// SWAP X, Y
			swap_trytes(*tryte_regs[second], *tryte_regs[third]);
			break;

		case 'j':
			// jXY - Trint pair operations
			switch (high_2)
			{
				case 0:
					// j(M-K)(M-m) - SET3 X, Y
					set_trint(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 1:
					// j(J-H)(M-m) - CMP3 X, Y
					compare_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 2:
					// j(G-E)(M-m) - ADD3 X, Y
					add_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 3:
					// j(D-B)(M-m) - MUL3 X, Y
					mult_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 4:
					// j(A-a)(M-m) - DIV3 X, Y
					div_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 5:
					// j(b-d)(M-m) - AND3 X, Y
					and_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 6:
					// j(e-g)(M-m) - OR3 X, Y
					or_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 7:
					// j(h-j)(M-m) - XOR3 X, Y
					xor_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				case 8:
					// j(k-m)(M-m) - SWAP3 X, Y
					swap_trints(*trint_regs[mid_2], *trint_regs[low_2]);
					break;
				
				default:
					halt_and_catch_fire();
					break;
			}
			break;

		case 'k':
			// kXY - miscellanous single Trint register
			switch (second)
			{
				case 'b':
				    // kbX - SET X, N
					set_trint_to_num(*trint_regs[low_2]);
					break;
				case 'a':
				    // kaX - ADD X, N
					add_num_to_trint(*trint_regs[low_2]);
					break;
				case 'c':
					// kcX - CMP X, N
					compare_trint_to_num(*trint_regs[low_2]);
					break;
				case 'd':
					// kdX - DIV X, N
					div_trint_by_num(*trint_regs[low_2]);
					break;
				case 'e':
					// keX - MUL X, N
					mult_trint_by_num(*trint_regs[low_2]);
					break;
				case 'f':
					// kfX - AND X, N
					and_trint_by_num(*trint_regs[low_2]);
					break;
				case 'g':
					// kgX - OR X, N
					or_trint_by_num(*trint_regs[low_2]);
					break;
				case 'h':
					// khX - XOR X, N
					xor_trint_by_num(*trint_regs[low_2]);
					break;
				case 'i':
					// kiX - INC X
					inc_trint(*trint_regs[low_2]);
					break;
				case 'I':
					// kIX - DEC X
					dec_trint(*trint_regs[low_2]);
					break;
				case 'A':
					// kAX - ABS X
					abs_trint(*trint_regs[low_2]);
					break;
				case 'B':
					// kBX - NOT X
					not_trint(*trint_regs[low_2]);
					break;
				case '0':
					// k0X - FLIP X
					flip_trint(*trint_regs[low_2]);
					break;
				case 'm':
					// kmX - SHL X, n
					shift_trint_left(*trint_regs[low_2]);
					break;
				case 'M':
					// kMX - SHR X, n
					shift_trint_right(*trint_regs[low_2]);
					break;
				default:
					halt_and_catch_fire();
					break;
			}
			break;

		case 'K':
			// Kxy - Tryte register & constant
			switch (second)
			{
				case 'a':
				    // KaX - ADD X, N
					add_num_to_tryte(*tryte_regs[third]);
					break;
				case 'b':
				    // KbX - SET X, N
					set_tryte_to_num(*tryte_regs[third]);
					break;
				case 'c':
					// KcX - CMP X, N
					compare_tryte_to_num(*tryte_regs[third]);
					break;
				case 'd':
					// KdX - DIV X, N
					div_tryte_by_num(*tryte_regs[third]);
					break;
				case 'e':
					// KeX - MUL X, N
					mult_tryte_by_num(*tryte_regs[third]);
					break;
				case 'f':
					// KfX - AND X, N
					and_tryte_by_num(*tryte_regs[third]);
					break;
				case 'g':
					// KgX - OR X, N
					or_tryte_by_num(*tryte_regs[third]);
					break;
				case 'h':
					// KhX - XOR X, N
					xor_tryte_by_num(*tryte_regs[third]);
					break;
				case 'i':
					// KiX - INC X
					inc_tryte(*tryte_regs[third]);
					break;
				case 'I':
					// KIX - DEC X
					dec_tryte(*tryte_regs[third]);
					break;
				case 'A':
					// KAX - ABS X
					abs_tryte(*tryte_regs[third]);
					break;
				case 'B':
					// KBX - NOT X
					not_tryte(*tryte_regs[third]);
					break;
				case '0':
					// K0X - FLIP X
					flip_tryte(*tryte_regs[third]);
					break;
				case 'm':
					// KmX - SHL X, n
					shift_tryte_left(*tryte_regs[third]);
					break;
				
				case 'M':
					// KMX - SHR X, n
					shift_tryte_right(*tryte_regs[third]);
					break;
				
				default:
					halt_and_catch_fire();
					break;
			}
			break;

		default:
			halt_and_catch_fire();
			break;
	}
}

/*
OPERATIONS
*/

// read/write 
void CPU::read_tryte(Tryte& y)
{
	Tryte const& add_x = _memory[_i_ptr + 1];
	y = _memory[add_x];
	_i_ptr += 2;
}
void CPU::read_trint(Trint<3>& y)
{
	Tryte& add_x = _memory[_i_ptr + 1];
	std::array<Tryte, 3> memory_trytes = { _memory[add_x], _memory[add_x + 1], _memory[add_x + 2] };
	y = Trint<3>(memory_trytes);
	_i_ptr += 2;
}
void CPU::write_tryte(Tryte& x)
{
	Tryte& add_y = _memory[_i_ptr + 1];
	_memory[add_y] = x;
	_i_ptr += 2;
}
void CPU::write_trint(Trint<3>& x)
{
	Tryte& add_x = _memory[_i_ptr + 1];
	_memory[add_x] = x[0];
	_memory[add_x + 1] = x[1];
	_memory[add_x + 2] = x[2];
	_i_ptr += 2;
}
void CPU::load()
{
	// disk address is converted from Tryte to an int between 0 and 19682
	int16_t disk_add_x = Tryte::get_int(_memory[_i_ptr + 1]) + 9841;
	int16_t n = Tryte::get_int(_memory[_i_ptr + 2]);
	Tryte& add_y = _memory[_i_ptr + 3];

	Tryte temp;
	std::string diskname = _disknames[_disk_num];
	std::fstream disk(diskname);

	if (n > 0)
	{
		size_t n_unsigned = n;
		// seek to required point in file
		disk.seekg(4 * static_cast<int>(disk_add_x));
		// now start copying
		for (size_t i = 0; i < n_unsigned; i++)
		{
			disk >> temp;
			_memory[add_y + i] = temp;
		}
	}
	else
	{
		// n <= 0, do nothing
	}
	_i_ptr += 4;
}
void CPU::save()
{
	Tryte& add_x = _memory[_i_ptr + 1];
	size_t n = Tryte::get_int(_memory[_i_ptr + 2]) + 9841;
	int16_t disk_add_y = Tryte::get_int(_memory[_i_ptr + 3]) + 9841;

	std::string diskname = _disknames[_disk_num];
	std::fstream disk(diskname);
	Tryte temp;

	if (n > 0)
	{
		// seek to required point in file
		disk.seekg(4 * static_cast<int>(disk_add_y));
		// now start copying
		for (size_t i = 0; i < n; i++)
		{
			temp = _memory[add_x + i];
			disk << temp; // overwrite data on disk
			disk << " ";
		}
	}
	else
	{
		// n <= 0, do nothing
	}
	_i_ptr += 4;
}
void CPU::print()
{
	size_t n = Tryte::get_int(_memory[_i_ptr + 1]) + 9841;
	Tryte& add_x = _memory[_i_ptr + 2];
	for (size_t i = 0; i < n; i++)
	{
		_console << _memory[add_x + i];
	}
	_i_ptr += 3;
}
void CPU::show_tryte(Tryte& a)
{
	_console << a;
	_i_ptr += 1;
}
void CPU::show_trint(Trint<3>& a)
{
	_console << a;
	_i_ptr += 1;
}
void CPU::tell_tryte(Tryte& a)
{
	char c[2] = { 0, 0 };
	_console >> c[0] >> c[1];
	int16_t tryte_value = 128 * static_cast<int16_t>(c[0]) + static_cast<int16_t>(c[1]) - 9841;
	a = Tryte(tryte_value);
	_i_ptr += 1;
}
void CPU::tell_trint(Trint<3>& a)
{
	char c[2] = { 0, 0 };
	for (size_t i = 0; i < 3; i++)
	{
		_console >> c[0] >> c[1];
		int16_t tryte_value = 128 * static_cast<int16_t>(c[0]) + static_cast<int16_t>(c[1]) - 9841;
		a[i] = Tryte(tryte_value);
	}
	_i_ptr += 1;
}
void CPU::peek_tryte(Tryte& a)
{
	a = _memory[_s_ptr - 1];
	_i_ptr += 1;
}
void CPU::peek_trint(Trint<3>& a)
{
	std::array<Tryte, 3> stack_trytes = { _memory[_s_ptr - 1], _memory[_s_ptr - 2], _memory[_s_ptr - 3] };
	a = Trint<3>(stack_trytes);
	_i_ptr += 1;
}
void CPU::fill()
{
	Tryte& add_x = _memory[_i_ptr + 1];
	size_t n = Tryte::get_int(_memory[_i_ptr + 2]) + 9841;
	Tryte k = _memory[_i_ptr + 3];
	for (size_t i = 0; i < n; i++)
	{
		_memory[add_x + i] = k;
	}
	_i_ptr += 4;
}
void CPU::mount(size_t n)
{
	if (n < _disknames.size())
	{
		_disk_num = n;
	}
	else
	{
		throw std::runtime_error("Tried to mount a disk that doesn't exist.\n");
	}
	_i_ptr += 1;
}
void CPU::set_display_mode(Tryte& a)
{
	std::array<int16_t, 3> a_array = Tryte::septavingt_array(a);
	int16_t last_digit = a_array[2] + 13;
	switch (last_digit)
	{
		case 0:
			_console.raw_mode();
			break;
		case 1:
			_console.ternary_mode();
			break;
		case 2:
			_console.number_mode();
			break;
		case 3:
			_console.dense_text_mode();
			break;
		case 4:
			_console.wide_text_mode();
			break;
		case 5:
			_console.graphics_mode();
			break;
		default:
			throw std::runtime_error("Unrecognised display mode.\n");
			break;
	}
	_i_ptr += 2;
}
void CPU::set_display_mode(Trint<3>& a)
{
	int16_t last_digit = Tryte::septavingt_array(a[2])[2] + 13;
	switch (last_digit)
	{
		case 0:
			_console.raw_mode();
			break;
		case 1:
			_console.ternary_mode();
			break;
		case 2:
			_console.number_mode();
			break;
		case 3:
			_console.dense_text_mode();
			break;
		case 4:
			_console.wide_text_mode();
			break;
		case 5:
			_console.graphics_mode();
			break;
		default:
			throw std::runtime_error("Unrecognised display mode.\n");
			break;
	}
	_i_ptr += 2;
}
void CPU::set_display_mode(size_t n)
{
	switch (n)
	{
		case 0:
			_console.raw_mode();
			break;
		case 1:
			_console.ternary_mode();
			break;
		case 2:
			_console.number_mode();
			break;
		case 3:
			_console.dense_text_mode();
			break;
		case 4:
			_console.wide_text_mode();
			break;
		case 5:
			_console.graphics_mode();
			break;
		default:
			throw std::runtime_error("Unrecognised display mode.\n");
			break;
	}
	_i_ptr += 1;
}
void CPU::get_display_mode(Tryte& a)
{
	a = _console.get_output_mode() - 13;
	_i_ptr += 1;
}
void CPU::get_display_mode(Trint<3>& a)
{
	a = _console.get_output_mode() - 13;
	_i_ptr += 1;
}
void CPU::push_tryte(Tryte& a)
{
	_memory[_s_ptr] = a;
	_s_ptr += 1;
	_i_ptr += 1;
}
void CPU::push_trint(Trint<3>& a)
{
	_memory[_s_ptr] = a[0];
	_memory[_s_ptr + 1] = a[1];
	_memory[_s_ptr + 2] = a[2];
	_s_ptr += 3;
	_i_ptr += 1;
}
void CPU::pop_tryte(Tryte& a)
{
	a = _memory[_s_ptr - 1];
	_s_ptr -= 1;
	_i_ptr += 1;
}
void CPU::pop_trint(Trint<3>& a)
{
	std::array<Tryte, 3> stack_trytes = { _memory[_s_ptr - 3], _memory[_s_ptr - 2], _memory[_s_ptr - 1] };
	a = Trint<3>(stack_trytes);
	_s_ptr -= 3;
	_i_ptr += 1;
}
void CPU::where(Tryte& x)
{
	x = _s_ptr;
	_i_ptr += 1;
}

// register handling
void CPU::set_trint_to_num(Trint<3>& a)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> new_trint(new_trint_array);
	a = new_trint;
	_i_ptr += 4;
}
void CPU::set_trint_to_addr(Trint<3>& a)
{
	Tryte& add_x = _memory[_i_ptr + 1];
	std::array<Tryte, 3> new_trint_array = { _memory[add_x], _memory[add_x + 1], _memory[add_x + 2] };
	Trint<3> new_trint(new_trint_array);
	a = new_trint;
	_i_ptr += 2;
}
void CPU::set_trint(Trint<3>& a, Trint<3>& y)
{
	a = y;
	_i_ptr += 1;
}
void CPU::set_tryte_to_num(Tryte& a)
{
	a = _memory[_i_ptr + 1];
	_i_ptr += 2;
}
void CPU::set_tryte_to_addr(Tryte& a)
{
	Tryte& add_x = _memory[_i_ptr + 1];
	a = _memory[add_x];
	_i_ptr += 2;
}
void CPU::set_tryte(Tryte& a, Tryte& y)
{
	a = y;
	_i_ptr += 1;
}
void CPU::swap_trints(Trint<3>& a, Trint<3>& b)
{
	Trint<3> temp = a;
	a = b;
	b = temp;
	_i_ptr += 1;
}
void CPU::swap_trytes(Tryte& a, Tryte& b)
{
	Tryte temp = a;
	a = b;
	b = temp;
	_i_ptr += 1;
}

// flag handling
void CPU::clear_compare()
{
	_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0"));
	_i_ptr += 1;
}
void CPU::clear_carry()
{
	_flags = Tryte::tritwise_mult(_flags, Tryte("+++++++0+"));
	_i_ptr += 1;
}
void CPU::clear_overflow()
{
	_flags = Tryte::tritwise_mult(_flags, Tryte("++++++0++"));
	_i_ptr += 1;
}
void CPU::set_priority(int16_t n)
{
	_flags = Tryte::tritwise_mult(_flags, Tryte("+++000+++"));
	_flags += (27 * n);
	_i_ptr += 1;
}
void CPU::check_priority()
{
	int16_t stored_priority = Tryte::get_int((_flags >> 6));
	int16_t current_priority = Tryte::get_int(Tryte::tritwise_mult(_flags, Tryte("000+++000")) >> 3);
	if (stored_priority > current_priority)
	{
		switch_thread(stored_priority);
	}
	else
	{
		_i_ptr += 1;
	}	
}

// arithmetic operations
void CPU::flip_tryte(Tryte& x)
{
	x = -x;
	_i_ptr += 1;
}
void CPU::flip_trint(Trint<3>& x)
{
	x = -x;
	_i_ptr += 1;
}
void CPU::inc_tryte(Tryte& x)
{
	x++;
	_i_ptr += 1;
}
void CPU::inc_trint(Trint<3>& x)
{
	x++;
	_i_ptr += 1;
}
void CPU::dec_tryte(Tryte& x)
{
	x--;
	_i_ptr += 1;
}
void CPU::dec_trint(Trint<3>& x)
{
	x--;
	_i_ptr += 1;
}
void CPU::add_trytes(Tryte& x, Tryte& y)
{
	std::array<Tryte, 2> temp = Tryte::add_with_carry(x, y, Tryte("000"));
	x = temp[1];
	// Y is overwritten with the carry.
	y = temp[0];
	// set carry flag
	_flags = Tryte::tritwise_mult(_flags, Tryte("mmj"));
	if (y > 0)
	{
		_flags += 3;
	}
	else if (y < 0)
	{
		_flags -= 3;
	}
	_i_ptr += 1;
}
void CPU::add_num_to_tryte(Tryte& x)
{
	Tryte num = _memory[_i_ptr + 1];
	x = Tryte::add_with_carry(x, num, Tryte("000"))[1];
	_i_ptr += 2;
}
void CPU::add_trints(Trint<3>& x, Trint<3>& y)
{
	x += y;
	_i_ptr += 1;
}
void CPU::add_num_to_trint(Trint<3>& x)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> num(new_trint_array);
	x += num;
	_i_ptr += 4;
}
void CPU::mult_trytes(Tryte& x, Tryte& y)
{
	std::array<Tryte, 2> temp = Tryte::mult(x, y);
	x = temp[1];
	// Y is overwritten with the carry.
	y = temp[0];
	// set carry flag
	_flags = Tryte::tritwise_mult(_flags, Tryte("mmj"));
	if (y > 0)
	{
		_flags += 3;
	}
	else if (y < 0)
	{
		_flags -= 3;
	}
	_i_ptr += 1;
}
void CPU::mult_tryte_by_num(Tryte& x)
{
	Tryte num = _memory[_i_ptr + 1];
	x = Tryte::mult(x, num)[1];
	_i_ptr += 2;
}
void CPU::mult_trints(Trint<3>& x, Trint<3>& y)
{
	x *= y;
	_i_ptr += 1;
}
void CPU::mult_trint_by_num(Trint<3>& x)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> num(new_trint_array);
	x *= num;
	_i_ptr += 4;
}
void CPU::div_trytes(Tryte& x, Tryte& y)
{
	try
	{
		std::array<Tryte, 2> div_result = Tryte::div(x, y);
		x = div_result[0];
		y = div_result[1];
		_i_ptr += 1;
	}
	catch(const std::runtime_error& e)
	{
		// divide by zero - set overflow flag and go to next operation
		_flags = Tryte::tritwise_mult(_flags, Tryte("mmd"));
		_flags += 9;
		_i_ptr += 1;
	}
}
void CPU::div_tryte_by_num(Tryte& x)
{
	Tryte num = _memory[_i_ptr + 1];

	try
	{
		std::array<Tryte, 2> div_result = Tryte::div(x, num);
		x = div_result[0];
		_i_ptr += 2;
	}
	catch(const std::runtime_error& e)
	{
		// divide by zero - set overflow flag and go to next operation
		_flags = Tryte::tritwise_mult(_flags, Tryte("mmd"));
		_flags += 9;
		_i_ptr += 2;
	}
}
void CPU::div_trints(Trint<3>& x, Trint<3>& y)
{
	try
	{
		std::array<Trint<3>, 2> div_result = Trint<3>::div(x, y);
		x = div_result[0];
		y = div_result[1];
		_i_ptr += 1;
	}
	catch(const std::runtime_error& e)
	{
		// divide by zero - set overflow flag and go to next operation
		_flags = Tryte::tritwise_mult(_flags, Tryte("mmd"));
		_flags += 9;
		_i_ptr += 1;
	}
}
void CPU::div_trint_by_num(Trint<3>& x)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> num(new_trint_array);
	try
	{
		std::array<Trint<3>, 2> div_result = Trint<3>::div(x, num);
		x = div_result[0];
		_i_ptr += 4;
	}
	catch(const std::runtime_error& e)
	{
		// divide by zero - set overflow flag and go to next operation
		_flags = Tryte::tritwise_mult(_flags, Tryte("mmd"));
		_flags += 9;
		_i_ptr += 4;
	}
}
void CPU::shift_tryte_left(Tryte& x)
{
	size_t n = Tryte::get_int(_memory[_i_ptr + 1]) + 9841;
	x << n;
	_i_ptr += 2;
}
void CPU::shift_trint_left(Trint<3>& x)
{
	size_t n = Tryte::get_int(_memory[_i_ptr + 1]) + 9841;
	x << n;
	_i_ptr += 2;
}
void CPU::shift_tryte_right(Tryte& x)
{
	size_t n = Tryte::get_int(_memory[_i_ptr + 1]) + 9841;
	x >> n;
	_i_ptr += 2;
}
void CPU::shift_trint_right(Trint<3>& x)
{
	size_t n = Tryte::get_int(_memory[_i_ptr + 1]) + 9841;
	x >> n;
	_i_ptr += 2;
}

// comparison
void CPU::compare_trytes(Tryte& x, Tryte& y)
{
	if (x < y)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) - 1;
	}
	else if (x > y)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) + 1;
	}
	else
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0"));
	}
	_i_ptr += 1;
}
void CPU::compare_tryte_to_num(Tryte& x)
{
	Tryte num = _memory[_i_ptr + 1];
	if (x < num)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) - 1;
	}
	else if (x > num)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) + 1;
	}
	else
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0"));
	}
	_i_ptr += 2;
}
void CPU::compare_trints(Trint<3>& x, Trint<3>& y)
{
	if (x < y)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) - 1;
	}
	else if (x > y)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) + 1;
	}
	else
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0"));
	}
	_i_ptr += 1;
}
void CPU::compare_trint_to_num(Trint<3>& x)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> num(new_trint_array);

	if (x < num)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) - 1;
	}
	else if (x > num)
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0")) + 1;
	}
	else
	{
		_flags = Tryte::tritwise_mult(_flags, Tryte("++++++++0"));
	}
	_i_ptr += 4;
}

// logic
void CPU::and_trytes(Tryte& x, Tryte& y)
{
	x &= y;
	_i_ptr += 1;
}
void CPU::and_tryte_by_num(Tryte& x)
{
	Tryte num = _memory[_i_ptr + 1];
	x &= num;
	_i_ptr += 2;
}
void CPU::and_trints(Trint<3>& x, Trint<3>& y)
{
	x &= y;
	_i_ptr += 1;
}
void CPU::and_trint_by_num(Trint<3>& x)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> num(new_trint_array);

	x &= num;
	_i_ptr += 4;
}
void CPU::or_trytes(Tryte& x, Tryte& y)
{
	x |= y;
	_i_ptr += 1;
}
void CPU::or_tryte_by_num(Tryte& x)
{
	Tryte num = _memory[_i_ptr + 1];
	x |= num;
	_i_ptr += 2;
}
void CPU::or_trints(Trint<3>& x, Trint<3>& y)
{
	x |= y;
	_i_ptr += 1;
}
void CPU::or_trint_by_num(Trint<3>& x)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> num(new_trint_array);

	x |= num;
	_i_ptr += 4;
}
void CPU::xor_trytes(Tryte& x, Tryte& y)
{
	x ^= y;
	_i_ptr += 1;
}
void CPU::xor_tryte_by_num(Tryte& x)
{
	Tryte num = _memory[_i_ptr + 1];
	x ^= num;
	_i_ptr += 2;
}
void CPU::xor_trints(Trint<3>& x, Trint<3>& y)
{
	x ^= y;
	_i_ptr += 1;
}
void CPU::xor_trint_by_num(Trint<3>& x)
{
	std::array<Tryte, 3> new_trint_array = { _memory[_i_ptr + 1], _memory[_i_ptr + 2], _memory[_i_ptr + 3] };
	Trint<3> num(new_trint_array);

	x ^= num;
	_i_ptr += 4;
}
void CPU::abs_tryte(Tryte& x)
{
	x = Tryte::abs(x);
	_i_ptr += 1;
}
void CPU::abs_trint(Trint<3>& x)
{
	x = Trint<3>::abs(x);
	_i_ptr += 1;
}
void CPU::not_tryte(Tryte& x)
{
	x = ~x;
	_i_ptr += 1;
}
void CPU::not_trint(Trint<3>& x)
{
	x = ~x;
	_i_ptr += 1;
}

// control flow
void CPU::noop()
{
	_i_ptr += 1;
}
void CPU::jump_if_zero()
{
	int16_t compare_flag = Tryte::get_int(Tryte::tritwise_mult(_flags, Tryte("00000000+")));
	if (compare_flag == 0)
	{
		_i_ptr = _memory[_i_ptr + 1];
	}
	else
	{
		_i_ptr += 2;
	}
}
void CPU::jump_if_neg()
{
	int16_t compare_flag = Tryte::get_int(Tryte::tritwise_mult(_flags, Tryte("00000000+")));
	if (compare_flag < 0)
	{
		_i_ptr = _memory[_i_ptr + 1];
	}
	else
	{
		_i_ptr += 2;
	}
}
void CPU::jump_if_pos()
{
	int16_t compare_flag = Tryte::get_int(Tryte::tritwise_mult(_flags, Tryte("00000000+")));
	if (compare_flag > 0)
	{
		_i_ptr = _memory[_i_ptr + 1];
	}
	else
	{
		_i_ptr += 2;
	}
}
void CPU::jump()
{
	_i_ptr = _memory[_i_ptr + 1];
}
void CPU::jump_and_store()
{
	_memory[_s_ptr] = _i_ptr;
	_s_ptr += 1;
	_i_ptr = _memory[_i_ptr + 1];
}
void CPU::pop_and_jump()
{
	Tryte temp = _memory[_s_ptr - 1];
	_s_ptr -= 1;
	_i_ptr = _memory[temp];
}
void CPU::switch_thread(size_t n)
{
	_i_ptr = _int_ptrs[n];
}
void CPU::set_interrupt_ptr(size_t n)
{
	_int_ptrs[n] = _memory[_i_ptr + 1];
	_i_ptr += 2;
}
void CPU::wait()
{
	bool waiting = true;
	int16_t current_priority = Tryte::get_int(Tryte::tritwise_mult(_flags, Tryte("000+++000")) >> 3);
	while (waiting)
	{
		int16_t stored_priority = Tryte::get_int(_flags >> 6);
		if (stored_priority > current_priority)
		{
			switch_thread(stored_priority);
			waiting = false;
		}
		else
		{
			_clock += 1;
		}		
	}
}
void CPU::halt_and_catch_fire()
{
	_on = false;
	_i_ptr += 1;
}

// public methods
void CPU::boot()
{
	_on = true;
	size_t i = 0;
	Tryte temp;
	std::fstream boot_disk(_disknames[0]);

	while (boot_disk >> temp)
	{
		_memory[i] = temp;
		i += 1;
	}
}
void CPU::run()
{
	while (_on)
	{
		fetch();
		decode_and_execute();
		_clock += 1;
	}
}
void CPU::step()
{
	fetch();
	decode_and_execute();
	_clock += 1;
}
void CPU::switch_off()
{
	_on = false;
}
bool CPU::is_on()
{
	return _on;
}
void CPU::dump()
{
	_console.raw_mode();
	_console << "Next instruction: " << _memory[_i_ptr] << '\n';
	_console << "Integer registers:\n";
	_console.number_mode();
	_console << "a = " << _a << " b = " << _b << " c = " << _c << '\n';
	_console << "d = " << _d << " e = " << _e << " g = " << _g << '\n';
	_console << "h = " << _h << " i = " << _i << " j = " << _j << '\n';
	_console.raw_mode();
	_console << "i_ptr = " << _i_ptr << '\n';
	_console << "s_ptr = " << _s_ptr << '\n';
	_console.ternary_mode();
	_console << "Flags: " << _flags << '\n';
	_FPU.dump();
	_console << '\n';
	_console.raw_mode();
}
void CPU::set_interrupt_priority(int16_t n)
{
	// clear stored priority
	_flags = Tryte::tritwise_mult(_flags, Tryte("000++++++"));

	// get new priority
	Tryte new_priority = (Tryte(n) << 6);
	_flags += new_priority;
}