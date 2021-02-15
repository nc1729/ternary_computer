#include <iostream>
#include <string>
#include <ciso646>
#include "Console.h"
#include "Trint.h"
#include "Tryte.h"

Console& Console::operator<<(Tryte& t)
{
	std::cout << process_tryte(t);
	return *this;
}
Console& Console::operator<<(Trint<3>& trint)
{
	std::cout << process_trint(trint);
	return *this;
}
Console& Console::operator>>(char input)
{
	// when stream fails, just feed in zeroes
	if (!(std::cin >> input))
	{
		input = 0;
	}
		
	return *this;
}


std::string Console::process_tryte(Tryte& t)
{
	int16_t tryte_int = Tryte::get_int(t);
	std::string out_string = "";
	uint8_t first_char = (tryte_int + 9841) / 128;
	uint8_t second_char = (tryte_int + 9841) % 128;
	// strip away zeroes in Tryte
	if (first_char == 0)
	{
		out_string += second_char;
		return out_string;
	}
	else if (second_char == 0)
	{
		out_string += first_char;
		return out_string;
	}
	else if (first_char == 0 and second_char == 0)
	{
		return out_string;
	}
	else
	{
		out_string += first_char;
		out_string += second_char;
		return out_string;
	}
	return out_string;
}
int64_t Console::process_trint(Trint<3>& trint)
{
	return Trint<3>::get_int(trint);
	
}
