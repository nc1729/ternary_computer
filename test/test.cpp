#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include "Float.h"

void test_float_constructor(double input_float, std::string expected_output)
{
    TFloat f = input_float;
    std::string expected = expected_output;
    std::stringstream str_stream;
    str_stream << f;
    std::string found = str_stream.str();

    bool test_result = (expected == found);
    if (not test_result)
    {
        std::cout << __func__ << " failed.\n";
        std::cout << "Expected: " << expected << '\n';
        std::cout << "Found: " << found << '\n';
    }
    assert(test_result);

}
void test_float_add(double input_float1, double input_float2, double expected_out)
{
    TFloat f1 = input_float1;
    TFloat f2 = input_float2;
    TFloat f_expected = expected_out;
    TFloat f_found = f1 + f2;
    bool test_result = (f_expected == f_found); 
    if (not test_result)
    {
        std::cout << __func__ << " failed.\n";
        std::cout << "Expected: " << f_expected << '\n';
        std::cout << "Found: " << f1 + f2 << '\n';
    }
    assert(test_result);
}
void test_float_mult(double input_float1, double input_float2, double expected_out)
{
    TFloat f1 = input_float1;
    TFloat f2 = input_float2;
    TFloat f_expected = expected_out;
    TFloat f_found = f1 * f2;
    bool test_result = (f_expected == f_found); 
    if (not test_result)
    {
        std::cout << __func__ << " failed.\n";
        std::cout << "Expected: " << f_expected << '\n';
        std::cout << "Found: " << f1 + f2 << '\n';
    }
    assert(test_result);
}
int main()
{
    test_float_constructor(1.0, "000i00000");
    test_float_add(1.0, 1.0, 2.0);
    test_float_mult(1.0, 1.0, 1.0);
}