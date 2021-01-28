#include "Tryte.h"
#include <ciso646>
#include <iostream>
#include <cassert>

void string_init_test()
{
    Tryte t{1000};
    Tryte t2{"aja"};
    Tryte t3{"0t00++0+00+"};
    Tryte t4{"0saja"};

    if (t == t2 and t == t3 and t == t4)
    {
        std::cout << "Yeah!\n";
    }
}

void reference_test()
{
    if (Tryte(100) == Tryte(100))
    {
        std::cout << "yep\n";
    }
}

void logical_test()
{
    Tryte a("++-+++++-");
    Tryte b("-+000-++0");
    std::cout << "a = " << a.ternary_string() << '\n';
    std::cout << "b = " << b.ternary_string() << '\n';

    Tryte c = a & b;
    std::cout << "a & b = " << c.ternary_string() << '\n';
    if (c == Tryte("-+-00-++-"))
    {
        std::cout << "and test passed!\n";
    }
    c = a | b;
    std::cout << "a | b = " << c.ternary_string() << '\n';
    if (c == Tryte("++0+++++0"))
    {
        std::cout << "or test passed!\n";
    }
    c = ~b;
    std::cout << "~b = " << c.ternary_string() << '\n';
    if (c == Tryte("+-000+--0"))
    {
        std::cout << "not test passed!\n";
    }
    c = a ^ b;
    std::cout << "a ^ b = " << c.ternary_string() << '\n';
    if (c == Tryte("+-000+--0"))
    {
        std::cout << "xor test passed!\n";
    }
}

void tritshift_test()
{
    Tryte a("0t000-000+0");
    std::cout << "a        = " << a.ternary_string() << '\n';
    std::cout << "(a << 1) = " << (a << 1).ternary_string() << '\n';
    std::cout << "(a << 5) = " << (a << 5).ternary_string() << '\n';
    std::cout << "(a >> 2) = " << (a >> 2).ternary_string() << '\n';
}

void add_test()
{
    Tryte a1 = 100;
    Tryte a2 = 57;
    std::cout << "a1 = " << a1 << ", a2 = " << a2 << '\n';
    std::cout << "a1 + a2 = " << a1 + a2 << '\n';

    a1 = 9841;
    a2 = 9841;
    std::cout << "a1 = " << a1 << ", a2 = " << a2 << '\n';
    std::cout << "a1 + a2 = " << a1 + a2 << '\n';

    a1 = 9841;
    a2 = 9841;
    std::cout << "a1 = " << a1 << ", a2 = " << a2 << '\n';
    std::cout << "a1 + a2 = " << a1 + a2 << '\n';
}

void mult_test()
{
    Tryte a1 = 100;
    Tryte a2 = 57;
    std::cout << "a1 = " << a1 << ", a2 = " << a2 << '\n';
    std::array<Tryte, 2> result = Tryte::mult(a1, a2);
    std::cout << "a1 + a2 = " << result[1] << '\n';
    std::cout << "Carry tryte = " << result[0] << '\n';

    a1 = 9841;
    a2 = 9841;
    std::cout << "a1 = " << a1 << ", a2 = " << a2 << '\n';
    result = Tryte::mult(a1, a2);
    std::cout << "a1 * a2 = " << result[1] << '\n';
    std::cout << "Carry tryte = " << result[0] << '\n';

    a1 = -9841;
    a2 = -1;
    std::cout << "a1 = " << a1 << ", a2 = " << a2 << '\n';
    result = Tryte::mult(a1, a2);
    std::cout << "a1 * a2 = " << result[1] << '\n';
    std::cout << "Carry tryte = " << result[0] << '\n';
}