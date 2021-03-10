#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include "Float.h"

void test_float_constructor()
{
    TFloat f1 = 1/3.0;
    std::string expected = "00Ai00000";
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
    TFloat f1 = 1.0;
    TFloat f2 = 3.0;
    TFloat f_expected = 4.0;
    TFloat f_found = f1 + f2;
    bool test_result = (f_expected == f_found); 
    if (not test_result)
    {
        std::cout << __func__ << " failed.\n";
        std::cout << "Expected: " << f_expected << '\n';
        std::cout << "Found: " << f1 + f2 << '\n';
    }
    assert(test_result);

    f1 = 1.0;
    f2 = -1.0;
    f_expected = 0.0;
    f_found = f1 + f2;
    test_result = (f_expected == f_found); 
    if (not test_result)
    {
        std::cout << __func__ << " failed.\n";
        std::cout << "Expected: " << f_expected << '\n';
        std::cout << "Found: " << f1 + f2 << '\n';
    }
    assert(test_result);
}
void test_float_mult()
{
    TFloat f1 = 1.0;
    TFloat f2 = 1.0;
    TFloat f_expected = 1.0;
    TFloat f_found = f1 * f2;
    bool test_result = (f_expected == f_found); 
    if (not test_result)
    {
        std::cout << __func__ << " failed.\n";
        std::cout << "Expected: " << f_expected << '\n';
        std::cout << "Found: " << f1 + f2 << '\n';
    }
    assert(test_result);
    
    f1 = 9.0;
    f2 = -6.0;
    f_expected = -54.0;
    f_found = f1 + f2;
    test_result = (f_expected == f_found); 
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
    test_float_add();
    test_float_mult();
}