#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include "Float.h"

void test_float_constructor()
{
    TFloat f1 = 5.0;
    std::string expected = "00ae0000";
    std::stringstream str_stream;
    str_stream << f1;
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
void test_float_add()
{
    TFloat f1 = 12.0;
    TFloat f2 = 233.0;
    TFloat f_expected = 245.0;
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
int main()
{
    test_float_constructor();
}