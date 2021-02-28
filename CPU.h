#pragma once
#include <string>
#include <map>
#include "Memory.h"
#include "Trint.h"
#include "Console.h"
#include "FPU.h"

class CPU
{
private:
	// reference to main memory
	Memory<19683> _memory;

	// disk filenames
	std::vector<std::string> _disknames;

	// console
	Console _console;

	// clock ticks (for timer)
	size_t _clock;

	// on/off switch
	bool _on;

	// registers
	Trint<3> _a;
	Trint<3> _b;
	Trint<3> _c;
	Trint<3> _d;
	Trint<3> _e;
	Trint<3> _g;
	Trint<3> _h;
	Trint<3> _i;
	Trint<3> _j;

	// register IDs 
	std::map<char, Tryte*> tryte_regs = { {'M', &_a[0]}, {'L', &_a[1]}, {'K', &_a[2]},
		{'J', &_b[0]}, {'I', &_b[1]}, {'H', &_b[2]}, {'G', &_c[0]}, {'F', &_c[1]}, {'E', &_c[2]},
		{'D', &_d[0]}, {'C', &_d[1]}, {'B', &_d[2]}, {'A', &_e[0]}, {'0', &_e[1]}, {'a', &_e[2]},
		{'b', &_g[0]}, {'c', &_g[1]}, {'d', &_g[2]}, {'e', &_h[0]}, {'f', &_h[1]}, {'g', &_h[2]},
		{'h', &_i[0]}, {'i', &_i[1]}, {'j', &_i[2]}, {'k', &_j[0]}, {'l', &_j[1]}, {'m', &_j[2]}};

	std::array<Trint<3>*, 9> trint_regs = { &_a, &_b, &_c, &_d, &_e, &_g, &_h, &_i, &_j };

	// instruction pointer
	Tryte _i_ptr;

	// stack pointer
	Tryte _s_ptr;

	// interrupt pointer array
	std::array<Tryte, 27> _int_ptrs;

	// current disk
	int16_t _disk_num;

	// current instruction
	Tryte _instr;

	// flags (for interrupts, logical results of calcs, etc)
	// layout:
	// stored_interrupt (3 trits) | current_interrupt (3 trits) | overflow | carry | compare
	Tryte _flags;

	// float processing unit (contains float registers)
	FPU _FPU = FPU(_memory, _console, _flags, _i_ptr, _s_ptr);

	// fetch the Tryte at the instruction pointer and set it as current instruction
	void fetch();
	// decode the current instruction and execute it
	void decode_and_execute();

	/*
	OPERATIONS
	*/

	/*
	read/write
	*/
	// READ $X, Y
	// Read from address X and store in register Y.
	void read_tryte(Tryte& y);
	// READ3 $X, Y
	// Read three Trytes starting at address X and store in Y
	void read_trint(Trint<3>& y);
	// WRITE X, $Y
	// Write contents of register X to address Y.
	void write_tryte(Tryte& x);
	// WRITE3 X, $Y
	// Write contents of Trint register X and store in addresses Y, Y+1, Y+2
	void write_trint(Trint<3>& x);
	// LOAD $X, n, $Y
	// Open device, navigate to address X, and copy Trytes at $X,$X+1,...,$X+n-1 to $Y,...$Y+n-1 in memory
	void load();
	// SAVE $X, n, $Y
	// Open device and copy n Trytes from memory ($X, $X+1, ... $X+n-1) onto device, starting at address Y.
	void save();
	// FILL $X, n, k
	// Fill the trytes $X, $X+1, ..., $X+n-1 with value k.
	void fill();
	// MNT n
	// Mount the nth device. All addresses will be relative to device n.
	void mount(size_t n);

	/*
	console management
	*/
	// DSET (A, n)
	// Set the display mode. 
	// n = 0: raw
	// n = 1: ternary
	// n = 2: number
	// n = 3: dense_text
	// n = 4: wide_text
	// n = 5: graphics
	// if a register is input, only the last three ternary digits are used
	void set_display_mode(Tryte& a);
	void set_display_mode(Trint<3>& a);
	void set_display_mode(size_t n);
	// DGET A
	// Get the display mode and put it in register A.
	void get_display_mode(Tryte& a);
	void get_display_mode(Trint<3>& a);
	// PRINT $X, n
	// Send n trytes, starting from $X in memory, to the console output.
	void print();
	// SHOW A
	// Send a Tryte or Trint register value directly to the console
	void show_tryte(Tryte& a);
	void show_trint(Trint<3>& a);
	// TELL A
	// Fill a Tryte/Trint register value with some chars from the console
	void tell_tryte(Tryte& a);
	void tell_trint(Trint<3>& a);

	/*
	stack management
	*/
    // PEEK A
	// Copy top element on stack to register, without popping it off.
	void peek_tryte(Tryte& a);
	void peek_trint(Trint<3>& a);
    // PUSH X
	// Pushes register X (one tryte or three) onto the stack, and increments the stack pointer accordingly
	void push_tryte(Tryte& a);
	void push_trint(Trint<3>& a);
	// POP X
	// Pops value from the stack into register X.
	void pop_tryte(Tryte& a);
	void pop_trint(Trint<3>& a);
	// WHERE X
	// Puts the location of the stack pointer into register X.
	void where(Tryte& x);

	/*
	register handling
	*/
	// SET Ax, ($X, n, Y)
	// set Tryte register Ax to contents of address X (or number n, or register Y).
	// wide numbers/registers will be truncated.
	void set_tryte_to_num(Tryte& a);
	void set_tryte_to_addr(Tryte& a);
	void set_tryte(Tryte& a, Tryte& y);
	// SET A, ($X, n, Y)
	// set Trint register A to contents of address {X, X+1, X+2} (or number n, or register Y)
	void set_trint_to_num(Trint<3>& a);
	void set_trint_to_addr(Trint<3>& a);
	void set_trint(Trint<3>& a, Trint<3>& y);
	// SWAP3 A, B
	// swap Trint registers A and B
	void swap_trints(Trint<3>& a, Trint<3>& b);
	// SWAP Ax, By
	// swap Tryte registers Ax and By
	void swap_trytes(Tryte& a, Tryte& b);

	/*
	flag handling
	*/
	// CCMP
	// Set the compare flag to 0.
	void clear_compare();
	// CCAR
	// Set the carry flag to 0.
	void clear_carry();
	// COVF
	// Set the overflow flag to 0.
	void clear_overflow();
	// PRI n
	// Sets the interrupt flag of the running process.
	// Flag is stored in 3 trits (can take value -13 < n < 13)
	void set_priority(int16_t n);
	// CHK
	// Compares interrupt flag of stored and running processes.
	// If stored interrupt beats current interrupt, CPU jumps to stored thread.
	void check_priority();

	/*
	integer arithmetic operations
	*/
	// FLIP X
	// Flip the sign of the contents of register X
	void flip_tryte(Tryte& x);
	// FLIP X
	// Flip the sign of the contents of register X
	void flip_trint(Trint<3>& x);
	// INC X
	// add 1 to register X
	void inc_tryte(Tryte& x);
	// INC X
	// add 1 to trint register X
	void inc_trint(Trint<3>& x);
	// DEC X
	// subtract 1 from register X
	void dec_tryte(Tryte& x);
	// DEC X
	// subtract 1 from trint register X
	void dec_trint(Trint<3>& x);
	// ADD X, Y
	// add the trytes X, Y, and store the result in X and the carry in register C0
	void add_trytes(Tryte& x, Tryte& y);
	void add_num_to_tryte(Tryte& x);
	// ADD X, Y
	// add the trints X and Y and store the result in X
	void add_trints(Trint<3>& x, Trint<3>& y);
	void add_num_to_trint(Trint<3>& x);
	// MUL X, Y
	// multiply the trytes X and Y and store the result in X and the carry in register C0
	void mult_trytes(Tryte& x, Tryte& y);
	void mult_tryte_by_num(Tryte& x);
	// MUL X, Y
	// multiply the trints X and Y and store the result in X
	void mult_trints(Trint<3>& x, Trint<3>& y);
	void mult_trint_by_num(Trint<3>& x);
	// DIV X, Y
	// multiply the trytes X and Y and store the result in X
	void div_trytes(Tryte& x, Tryte& y);
	void div_tryte_by_num(Tryte& x);
	// DIV X, Y
	// multiply the trints X and Y and store the result in X
	void div_trints(Trint<3>& x, Trint<3>& y);
	void div_trint_by_num(Trint<3>& x);
	// SHL X, n
	// shift the tryte X left by n trits
	void shift_tryte_left(Tryte& x);
	// SHL X, n
	// shift the trint X left by n trits
	void shift_trint_left(Trint<3>& x);
	// SHR X, n
	// shift the tryte X right by n trits
	void shift_tryte_right(Tryte& x);
	// SHR X, n
	// shift the trint X right by n trits
	void shift_trint_right(Trint<3>& x);

	/* 
	comparison
	*/
	// CMP X, Y
	// compare X and Y:
	// if X < Y, set compare flag to -; 
	// if X == Y, set compare flag to 0;
	// if X > Y, set compare flag to +;
	void compare_trytes(Tryte& x, Tryte& y);
	void compare_tryte_to_num(Tryte& x);
	// CMP3 X, Y
	// compare X and Y:
	// if X < Y, set compare flag to -; 
	// if X == Y, set compare flag to 0;
	// if X > Y, set compare flag to +;
	void compare_trints(Trint<3>& x, Trint<3>& y);
	void compare_trint_to_num(Trint<3>& x);

	/*
	logic
	*/
	// AND X, Y
	// compute X & Y and store result in X
	void and_trytes(Tryte& x, Tryte& y);
	void and_tryte_by_num(Tryte& x);
	// AND X, Y
	// compute X & Y and store result in X
	void and_trints(Trint<3>& x, Trint<3>& y);
	void and_trint_by_num(Trint<3>& x);
	// OR X, Y
	// compute X | Y and store result in X
	void or_trytes(Tryte& x, Tryte& y);
	void or_tryte_by_num(Tryte& x);
	// OR X, Y
	// compute X | Y and store result in X
	void or_trints(Trint<3>& x, Trint<3>& y);
	void or_trint_by_num(Trint<3>& x);
	// XOR X, Y
	// compute X ^ Y and store result in X
	void xor_trytes(Tryte& x, Tryte& y);
	void xor_tryte_by_num(Tryte& x);
	// XOR X, Y
	// compute X ^ Y and store result in X
	void xor_trints(Trint<3>& x, Trint<3>& y);
	void xor_trint_by_num(Trint<3>& x);
	// ABS X
	// if negative, flip sign of X, else do nothing
	void abs_tryte(Tryte& x);
	void abs_trint(Trint<3>& x);
	// NOT X
	// compute ~X and store result in X
	void not_tryte(Tryte& x);
	void not_trint(Trint<3>& x);

	/*
	control flow
	*/
	// NOOP
	// Do nothing.
	void noop();
	// JPZ $X
	// Checks the compare flag; if zero, process jumps to address X.
	void jump_if_zero();
	// JPN $X
	// Checks the compare flag; if negative, process jumps to address X.
	void jump_if_neg();
	// JPP $X
	// Checks the compare flag; if positive, process jumps to address X.
	void jump_if_pos();
	// JP $X
	// Jump to address X, unconditionally.
	void jump();
	// JPS $X
	// Jump to address X and push instruction pointer onto stack.
	void jump_and_store();
	// PJP
	// Pop from stack, and go to that location in memory
	void pop_and_jump();
	// THD n
	// Jump execution to the nth thread (_i_ptr goes to _int_ptrs[n])
	void switch_thread(size_t n);
	// INT n, $X
	void set_interrupt_ptr(size_t n);
	// HALT
	// Stop the CPU. CPU will have to be turned on from outside.
	void halt_and_catch_fire();
	// WAIT
	// do nothing but compare interrupts. Useful while waiting for input.
	void wait();


public:
	CPU(Memory<19683>& memory, std::vector<std::string>& disk_names);
	void boot();
	void run();
	void step();
	void switch_off();
	bool is_on();
	void current_instr();
	void dump();
	void set_interrupt_priority(int16_t n);
};