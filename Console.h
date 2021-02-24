#pragma once
#include <iostream>
#include <string>
#include "Tryte.h"
#include "Trint.h"
#include "Float.h"
class Console
{
private:
	// routine for decoding raw Trytes into pairs of ASCII characters
	// Tryte t = (128a + b - 9841), where 0 <= a, b < 128 is the basic ASCII char set
	// pad spare values with b = 0
	std::string process_tryte(Tryte& t);
	int64_t process_trint(Trint<3>& trint);
	double process_float(TFloat& tfloat);

public:
	Console& operator<<(Tryte& t);
	Console& operator<<(Trint<3>& trint);
	Console& operator<<(TFloat& tfloat);
	Console& operator>>(char input);
};