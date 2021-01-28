#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "Tryte.h"

template <size_t n>
class Memory
{
private:
	std::vector<Tryte> _memory;

public:
	Memory()
	{
		_memory.resize(n, Tryte("000"));
	}
	Tryte& operator[](int const i)
	{
		return _memory[i + ((n - 1) / 2)];
	}
	Tryte const& operator[](int const i) const
	{
		return _memory[i + ((n - 1) / 2)];
	}
	Tryte& operator[](Tryte t)
	{
		int16_t tryte_val = t.get_int();
		return _memory[tryte_val + 9841];
	}
	Tryte const& operator[](Tryte& t) const
	{
		int16_t tryte_val = t.get_int();
		return _memory[tryte_val + 9841];
	}
	void dump_to_file(std::string& dump_filename)
	{
		// open dump file
		std::ofstream dump_file(dump_filename);

		// and dump contents of memory there
		for (size_t i = 0; i < n; i++)
		{
			dump_file << _memory[i];
		}

		// close file
		dump_file.close();
	}
};
