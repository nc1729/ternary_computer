#include "CPU.h"
#include "Console.h"
#include <vector>
#include <string>
#include <array>
#include <fstream>
#include <iostream>
#include <stdexcept>

CPU::CPU(Memory<19683>* memory, std::vector<std::string>& disknames)
{
	_memory = *memory;
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

void CPU::execute()
{
	std::string instruction = _instr.septavingt_string();
	char first = instruction[0];
	char second = instruction[1];
	char third = instruction[2];
	std::array<int16_t, 9> tern_array = _instr.ternary_array();
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
				// 0mn - MOUNT n
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
				case '0':
					// a0 - PRINT $X, N
					print();
					break;

				case 'A':
					// aAX - SHOW X
					show_tryte(*tryte_regs[third]);
					break;

				case 'a':
					// aaX - SHOW X
					show_trint(*trint_regs[low_2]);
					break;

				case 'B':
					// aBX - TELL X
					tell_tryte(*tryte_regs[third]);
					break;

				case 'b':
					// abX - SHOW X
					tell_trint(*trint_regs[low_2]);
					break;

				case 'C':
					// aCY - READ $X, Y
					read_tryte(*tryte_regs[third]);
					break;

				case 'c':
					// acY - READ $X, Y
					read_trint(*trint_regs[low_2]);
					break;

				case 'D':
					// aDX - WRITE X, $Y
					write_tryte(*tryte_regs[third]);
					break;

				case 'd':
					// adX - WRITE X, $Y
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
				case '0':
					// k0X - SET X, N
					set_trint_to_num(*trint_regs[low_2]);
					break;

				case 'a':
					// kaX - INC X
					inc_trint(*trint_regs[low_2]);
					break;
				
				case 'A':
					// kAX - DEC X
					dec_trint(*trint_regs[low_2]);
					break;
				
				case 'c':
					// kcX - NOT X
					not_trint(*trint_regs[low_2]);
					break;
				
				case 'f':
					// kfX - FLIP X
					flip_trint(*trint_regs[low_2]);
					break;
				
				case 'M':
					// kMX - SHL X, n
					shift_trint_left(*trint_regs[low_2]);
					break;
				
				case 'm':
					// kmX - SHR X, n
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
				case '0':
					// K0X - SET X, N
					set_tryte_to_num(*tryte_regs[third]);
					break;

				case 'a':
					// KaX - INC X
					inc_tryte(*tryte_regs[third]);
					break;
				
				case 'A':
					// KAX - DEC X
					dec_tryte(*tryte_regs[third]);
					break;
				
				case 'c':
					// KcX - NOT X
					not_tryte(*tryte_regs[third]);
					break;
				
				case 'f':
					// KfX - FLIP X
					flip_tryte(*tryte_regs[third]);
					break;
				
				case 'M':
					// KMX - SHL X, n
					shift_tryte_left(*tryte_regs[third]);
					break;
				
				case 'm':
					// KmX - SHR X, n
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
	Tryte& add_y = _memory[_i_ptr + 1];
	_memory[add_y] = x[0];
	_memory[add_y + 1] = x[1];
	_memory[add_y + 2] = x[2];
	_i_ptr += 2;
}
void CPU::load()
{
	// disk address is converted from Tryte to an int between 0 and 19682
	int16_t disk_add_x = _memory[_i_ptr + 1].get_int() + 9841;
	int16_t n = _memory[_i_ptr + 2].get_int();
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
	size_t n = _memory[_i_ptr + 2].get_int() + 9841;
	int16_t disk_add_y = _memory[_i_ptr + 3].get_int() + 9841;

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
	size_t n = _memory[_i_ptr + 1].get_int() + 9841;
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
	size_t n = _memory[_i_ptr + 2].get_int() + 9841;
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
	int16_t stored_priority = (_flags >> 6).get_int();
	int16_t current_priority = (Tryte::tritwise_mult(_flags, Tryte("000+++000")) >> 3).get_int();
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
void CPU::add_trints(Trint<3>& x, Trint<3>& y)
{
	x += y;
	_i_ptr += 1;
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
void CPU::mult_trints(Trint<3>& x, Trint<3>& y)
{
	x *= y;
	_i_ptr += 1;
}
void CPU::div_trytes(Tryte& x, Tryte& y)
{
	// to be implemented!
	x += 1;
	x -= 1;
	y += 1;
	y -= 1;
	_i_ptr += 1;
}
void CPU::div_trints(Trint<3>& x, Trint<3>& y)
{
	// to be implemented!
	x += 1;
	x -= 1;
	y += 1;
	y -= 1;
	_i_ptr += 1;
}
void CPU::shift_tryte_left(Tryte& x)
{
	size_t n = _memory[_i_ptr + 1].get_int() + 9841;
	x << n;
	_i_ptr += 2;
}
void CPU::shift_trint_left(Trint<3>& x)
{
	size_t n = _memory[_i_ptr + 1].get_int() + 9841;
	x << n;
	_i_ptr += 2;
}
void CPU::shift_tryte_right(Tryte& x)
{
	size_t n = _memory[_i_ptr + 1].get_int() + 9841;
	x >> n;
	_i_ptr += 2;
}
void CPU::shift_trint_right(Trint<3>& x)
{
	size_t n = _memory[_i_ptr + 1].get_int() + 9841;
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
void CPU::compare_trints(Trint<3> x, Trint<3> y)
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

// logic
void CPU::and_trytes(Tryte& x, Tryte& y)
{
	x &= y;
	_i_ptr += 1;
}
void CPU::and_trints(Trint<3> x, Trint<3> y)
{
	x &= y;
	_i_ptr += 1;
}
void CPU::or_trytes(Tryte& x, Tryte& y)
{
	x |= y;
	_i_ptr += 1;
}
void CPU::or_trints(Trint<3> x, Trint<3> y)
{
	x |= y;
	_i_ptr += 1;
}
void CPU::xor_trytes(Tryte& x, Tryte& y)
{
	x ^= y;
	_i_ptr += 1;
}
void CPU::xor_trints(Trint<3> x, Trint<3> y)
{
	x ^= y;
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
	int16_t compare_flag = Tryte::tritwise_mult(_flags, Tryte("00000000+")).get_int();
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
	int16_t compare_flag = Tryte::tritwise_mult(_flags, Tryte("00000000+")).get_int();
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
	int16_t compare_flag = Tryte::tritwise_mult(_flags, Tryte("00000000+")).get_int();
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
	int16_t current_priority = (Tryte::tritwise_mult(_flags, Tryte("000+++000")) >> 3).get_int();
	while (waiting)
	{
		int16_t stored_priority = (_flags >> 6).get_int();
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
		execute();
		_clock += 1;
	}
}
void CPU::step()
{
	fetch();
	execute();
	_clock += 1;
}
bool CPU::is_on()
{
	return _on;
}
void CPU::dump()
{
	std::cout << "This instruction: " << _instr << '\n';
	std::cout << "Registers:\n";
	std::cout << "\na = ";
	_console << _a;
	std::cout << "\nb = ";
	_console << _b;
	std::cout << "\nc = ";
	_console << _c;
	std::cout << "\nd = ";
	_console << _d;
	std::cout << "\ne = ";
	_console << _e;
	std::cout << "\nf = ";
	_console << _f;
	std::cout << "\ng = ";
	_console << _g;
	std::cout << "\nh = ";
	_console << _h;
	std::cout << "\ni = ";
	_console << _i;
	std::cout << "\ni_ptr = " << _i_ptr << '\n';
	std::cout << "\ns_ptr = " << _s_ptr << '\n';
	std::cout << "Flags: " << _flags << '\n';
}
void CPU::set_interrupt_priority(int16_t n)
{
	// clear stored priority
	_flags = Tryte::tritwise_mult(_flags, Tryte("000++++++"));

	// get new priority
	Tryte new_priority = (Tryte(n) << 6);
	_flags += new_priority;
}