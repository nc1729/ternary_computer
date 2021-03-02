#include <iostream>
#include <string>
#include <ciso646>
#include "Console.h"
#include "Trint.h"
#include "Tryte.h"
#include "Float.h"

Console::Console()
{
	// on console start, set to raw mode (all Trytes in raw septavingtesmal form)
	_output_mode = OutputMode::raw;
}
Console& Console::operator<<(Tryte& t)
{
	if (_output_mode == OutputMode::raw)
	{
		std::cout << Tryte::septavingt_string(t);
	}
	else if (_output_mode == OutputMode::ternary)
	{
		std::cout << Tryte::ternary_string(t);
	}
	else if (_output_mode == OutputMode::number)
	{
		std::cout << Tryte::get_int(t);
	}
	else if (_output_mode == OutputMode::dense_text)
	{
		int16_t tryte_int = Tryte::get_int(t);
		std::string out_string = "";
		uint8_t first_char = (tryte_int + 9841) / 128;
		uint8_t second_char = (tryte_int + 9841) % 128;
		// strip away zeroes in Tryte
		if (first_char == 0)
		{
			out_string += second_char;
			std::cout << out_string;
		}
		else if (second_char == 0)
		{
			out_string += first_char;
			std::cout << out_string;
		}
		else if (first_char == 0 and second_char == 0)
		{
			std::cout << out_string;
		}
		else
		{
			out_string += first_char;
			out_string += second_char;
			std::cout << out_string;
		}
	}
	else if (_output_mode == OutputMode::wide_text)
	{
		char16_t out_char = (Tryte::get_int(t) + 9841);
		std::string out_string;
		out_string += out_char;
		std::cout << out_string;	
	}
	else if (_output_mode == OutputMode::graphics)
	{
		// graphics
	}
	return *this;
}
Console& Console::operator<<(Trint<3>& trint)
{
	if (_output_mode == OutputMode::raw)
	{
		std::cout << Tryte::septavingt_string(trint[0]) << ' ';
		std::cout << Tryte::septavingt_string(trint[1]) << ' ';
		std::cout << Tryte::septavingt_string(trint[2]);
	}
	else if (_output_mode == OutputMode::ternary)
	{
		std::cout << Tryte::ternary_string(trint[0]) << ' ';
		std::cout << Tryte::ternary_string(trint[1]) << ' ';
		std::cout << Tryte::ternary_string(trint[2]);
	}
	else if (_output_mode == OutputMode::number)
	{
		std::cout << Trint<3>::get_int(trint);
	}
	else if (_output_mode == OutputMode::dense_text)
	{
		// dense text
	}
	else if (_output_mode == OutputMode::wide_text)
	{
		// wide text
	}
	else if (_output_mode == OutputMode::graphics)
	{
		// graphics
	}
	return *this;
}
Console& Console::operator<<(TFloat& tfloat)
{
	if (_output_mode == OutputMode::raw)
	{
		Trint<1> tfloat_exponent = TFloat::get_exponent(tfloat);
		Trint<2> tfloat_mantissa = TFloat::get_mantissa(tfloat);
		std::cout << Tryte::septavingt_string(tfloat_exponent[0]) << ' ';
		std::cout << Tryte::septavingt_string(tfloat_mantissa[0]) << ' ';
		std::cout << Tryte::septavingt_string(tfloat_mantissa[1]) << ' ';
	}
	else if (_output_mode == OutputMode::ternary)
	{
		Trint<1> tfloat_exponent = TFloat::get_exponent(tfloat);
		Trint<2> tfloat_mantissa = TFloat::get_mantissa(tfloat);
		std::cout << Tryte::ternary_string(tfloat_exponent[0]) << ' ';
		std::cout << Tryte::ternary_string(tfloat_mantissa[0]) << ' ';
		std::cout << Tryte::ternary_string(tfloat_mantissa[1]) << ' ';
	}
	else if (_output_mode == OutputMode::number)
	{
		std::cout << TFloat::get_double(tfloat);
	}
	else if (_output_mode == OutputMode::dense_text)
	{
		// do nothing, can't interpret a float as chars
	}
	else if (_output_mode == OutputMode::wide_text)
	{
		// do nothing, can't interpret a float as chars
	}
	else if (_output_mode == OutputMode::graphics)
	{
		// do nothing, can't interpret a float as graphics
	}
	return *this;
}
Console& Console::operator<<(std::string out_string)
{
	std::cout << out_string;
	return *this;
}
Console& Console::operator<<(char c)
{
	std::cout << c;
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
int16_t Console::get_output_mode()
{
	if (_output_mode == OutputMode::raw)
	{
		return 0;
	}
	else if (_output_mode == OutputMode::ternary)
	{
		return 1;
	}
	else if (_output_mode == OutputMode::number)
	{
		return 2;
	}
	else if (_output_mode == OutputMode::dense_text)
	{
		return 3;
	}
	else if (_output_mode == OutputMode::wide_text)
	{
		return 4;
	}
	else if (_output_mode == OutputMode::graphics)
	{
		return 5;
	}
	else
	{
		// this should never happen
		return -1;
	}
	
}
void Console::raw_mode()
{
	_output_mode = OutputMode::raw;
}
void Console::ternary_mode()
{
	_output_mode = OutputMode::ternary;
}
void Console::number_mode()
{
	_output_mode = OutputMode::number;
}
void Console::dense_text_mode()
{
	_output_mode = OutputMode::dense_text;
}
void Console::wide_text_mode()
{
	_output_mode = OutputMode::wide_text;
}
void Console::graphics_mode()
{
	_output_mode = OutputMode::graphics;
}