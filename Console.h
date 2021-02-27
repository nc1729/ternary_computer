#pragma once
#include <iostream>
#include <string>
#include "Tryte.h"
#include "Trint.h"
#include "Float.h"
class Console
{
private:
	enum class OutputMode {
		// raw: all trytes sent to console will be printed in septavingtesmal form
		raw,
		// ternary: all trytes sent to console will be printed in ternary form
		ternary,
		// number: Trytes, Trints and TFloats will be printed in their numerical form
		number,
		// dense_text: each Tryte sent to console will be converted into two ASCII chars and printed
		dense_text,
		// wide_text: each Tryte will be converted to a single char and printed
		wide_text,
		// graphics: to be implemented later, convert sets of Trytes to ANSI colour codes etc
		graphics
	} _output_mode;

public:
	Console();
	Console& operator<<(Tryte& t);
	Console& operator<<(Trint<3>& trint);
	Console& operator<<(TFloat& tfloat);
	Console& operator<<(char c);
	Console& operator<<(std::string out_string);
	Console& operator>>(char input);

	// output mode
	void raw_mode();
	void ternary_mode();
	void number_mode();
	void dense_text_mode();
	void wide_text_mode();
	void graphics_mode();
};