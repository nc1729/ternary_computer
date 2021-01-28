#include "Tryte.h"
#include <ciso646> // for and & or in Visual Studio
#include <cstdint> // for int16_t
#include <algorithm> // for std::reverse (reverse string), std::min
#include <string> // for std::string
#include <vector> // for std::vector
#include <array> // for std::array
#include <stdexcept> // for std::runtime_error
#include <iostream> // for std::ostream

const std::string Tryte::ternary_chars = "-0+";
const std::string Tryte::septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm";

std::string Tryte::ternary_string() const
{
    std::string output(9, '0');
    std::array<int16_t, 9> tern_rep = ternary_array();

    // loop over the ternary array and construct the string
    for (int i = 0; i < 9; i++)
    {
        output[i] = Tryte::ternary_chars[1 + tern_rep[i]];
    }
    return output;
}
std::string Tryte::septavingt_string() const
{
    std::string output(3, '0');
    std::array<int16_t, 3> sept_rep = septavingt_array();

    // loop over septavingt array and construct the string
    for (int i = 0; i < 3; i++)
    {
        output[i] = Tryte::septavingt_chars[13 + sept_rep[i]];
    }
    return output;
}
std::array<int16_t, 9> Tryte::ternary_array() const
{
    std::array<int16_t, 9> output;
    int16_t dividend = abs(this->m_tryte); // make positive for algorithm
    int16_t remainder = 0;

    for (size_t i = 0; i < 9; i++)
    {
        remainder = dividend % 3;
        dividend = dividend / 3;
        // handle shift for balanced ternary representation
        if (remainder == 2)
        {
            dividend++;
            remainder = -1;
        }
        // fill output array backwards (algorithm finds least significant digit first)
        output[8 - i] = remainder;
    }

    // flip the sign back if necessary
    if (this->m_tryte < 0)
    {
        for (auto &trit : output)
        {
            trit = -trit;
        }
    }

    return output;
}
std::array<int16_t, 3> Tryte::septavingt_array() const
{
    std::array<int16_t, 3> output;
    // First generate the ternary rep
    std::array<int16_t, 9> tern = ternary_array();
    std::array<int16_t, 3> trits;

    for (size_t i = 0; i < 3; i++)
    {
        // take a subarray of three trits
        trits = {tern[3*i], tern[3*i + 1], tern[3*i + 2]};

        // convert to decimal value between -13 and 13 inclusive.
        output[i] = 9 * trits[0] + 3 * trits[1] + trits[2];
    }

    return output;
}
int16_t Tryte::get_int() const
{
    return m_tryte;
}
int16_t Tryte::ternary_array_to_int(std::array<int16_t, 9> ternary_array)
{
    int16_t power_of_3 = 1;
    int16_t output = 0;
    
    for (size_t i = 0; i < 9; i++)
    {
        output += power_of_3 * ternary_array[8 - i];
        power_of_3 *= 3; 
    }
    return output;
}
int16_t Tryte::septavingt_array_to_int(std::array<int16_t, 3> septavingt_array)
{
    int16_t power_of_27 = 1;
    int16_t output = 0;
    
    for (size_t i = 0; i < 3; i++)
    {
        output += power_of_27 * septavingt_array[2 - i];
        power_of_27 *= 27; 
    }
    return output;
}
int16_t Tryte::ternary_string_to_int(std::string t_string)
{
    int16_t output{0};
    int64_t power_of_three{1};

    // start by reversing the string
    std::reverse(t_string.begin(), t_string.end());

    // loop over string
    for (auto &c: t_string)
    {
        size_t pos = Tryte::ternary_chars.find(c);
        output += (pos - 1) * power_of_three;

        // it's cheaper to multiply by three each time rather than
        // compute pow(3, i)? but I might be overthinking it
        power_of_three *= 3;
    }

    return output;
}
int16_t Tryte::septavingt_string_to_int(std::string s_string)
{
    int16_t output = 0;
    int64_t power_of_27 = 1;

    // start by reversing the string
    std::reverse(s_string.begin(), s_string.end());

    for (auto &c: s_string)
    {
        size_t pos = Tryte::septavingt_chars.find(c);
        output += (pos - 13) * power_of_27;
        power_of_27 *= 27;
    }

    return output;
}
int16_t Tryte::truncate_int(int64_t const& n)
{
    // make input positive; it's just easier
    int64_t dividend = abs(n);

    int16_t remainder = 0;
    std::vector<int16_t> values;
    int16_t output = 0;
    while (dividend != 0)
    {
        remainder = dividend % 27;
        dividend = dividend / 27;
        // handle shift for balanced septavingtesmal representation
        if (remainder > 13)
        {
            dividend++;
            remainder -= 27;
        }
        else if (remainder < -13)
        {
            dividend--;
            remainder += 27;
        }
        values.push_back(remainder);
    }

    // only need first three characters of septavingtesmal rep
    size_t length = (values.size() > 3) ? 3 : values.size();

    int16_t power_of_27 = 1;

    for (size_t i = 0; i < length; i++)
    {
        output += values[i] * power_of_27;
        power_of_27 *= 27;
    }

    // fix minus sign, if necessary (advantage of balanced ternary)
    if (n < 0) output = -output;

    return output;
}
std::array<int16_t, 2> Tryte::carry_handler(int16_t n)
{
    std::array<int16_t, 2> output;

    if (n > 0)
    {
        output[0] = (n + 13) / 27;
        output[1] = n - ((n + 13) / 27) * 27;
    }
    else
    {
        output[0] = (n - 13) / 27;
        output[1] = n - ((n - 13) / 27) * 27;
    }

    return output;    
}

Tryte::Tryte()
{
    m_tryte = 0;
}
Tryte::Tryte(int64_t const x)
{
    m_tryte = truncate_int(x);
}
Tryte::Tryte(std::string tryte_string)
{
    m_tryte = 0;
    if (tryte_string.length() == 3)
    {
        // septavingt string
        // loop through characters
        for (size_t i = 0; i < 3; i++)
        {
            char c = tryte_string[i];
            std::array<int16_t, 3> powers_of_27 = { 729, 27, 1 };
            size_t septavingt_pos = Tryte::septavingt_chars.find(c);
            if (septavingt_pos != std::string::npos)
            {
                m_tryte += powers_of_27[i] * (septavingt_pos - 13);
            }
            else
            {
                throw std::runtime_error("Invalid string for Tryte initialisation.");
            }
        }
    }
    else if (tryte_string.length() == 9)
    {
        for (size_t i = 0; i < 9; i++)
        {
            char c = tryte_string[i];
            std::array<int16_t, 9> powers_of_3 = { 6561, 2187, 729, 243, 81, 27, 9, 3, 1 };
            size_t ternary_pos = Tryte::ternary_chars.find(c);
            if (ternary_pos != std::string::npos)
            {
                m_tryte += powers_of_3[i] * (ternary_pos - 1);
            }
            else
            {
                throw std::runtime_error("Invalid string for Tryte initialisation.");
            }
        }
    }
    else
    {
        throw std::runtime_error("Invalid string for Tryte initialisation.");
    }
    
}
Tryte::Tryte(std::array<int16_t, 3>& sep_array)
{
    m_tryte = 729 * sep_array[0] + 27 * sep_array[1] + sep_array[2];
}
Tryte::Tryte(std::array<int16_t, 9>& tern_array)
{
    std::array<int16_t, 9> powers_of_3 = { 6561, 2187, 729, 243, 81, 27, 9, 3, 1 };
    m_tryte = 0;
    for (size_t i = 0; i < 9; i++)
    {
        m_tryte = powers_of_3[i] * tern_array[i];
    }
}
Tryte::Tryte(Tryte const& other)
{
    m_tryte = other.m_tryte;
}

Tryte& Tryte::operator++()
{
    int64_t x = this->m_tryte;
    x++;
    this->m_tryte = truncate_int(x);
    return *this;
}
Tryte Tryte::operator++(int)
{
    Tryte temp(*this); // copy
    operator++(); // pre-increment
    return temp; // return old value
}
Tryte& Tryte::operator--()
{
    int64_t x = this->m_tryte;
    x--;
    this->m_tryte = truncate_int(x);
    return *this;
}
Tryte Tryte::operator--(int)
{
    Tryte temp(*this);
    operator--();
    return temp;
}

bool Tryte::operator==(Tryte const& other)
{
    return this->m_tryte == other.m_tryte;
}
bool Tryte::operator!=(Tryte const& other)
{
    return this->m_tryte != other.m_tryte;
}
bool Tryte::operator<(Tryte const& other)
{
    return this->m_tryte < other.m_tryte;
}
bool Tryte::operator<=(Tryte const& other)
{
    return this->m_tryte <= other.m_tryte;
}
bool Tryte::operator>(Tryte const& other)
{
    return this->m_tryte > other.m_tryte;
}
bool Tryte::operator>=(Tryte const& other)
{
    return this->m_tryte >= other.m_tryte;
}


Tryte Tryte::operator&(Tryte const& other)
{
    /*
    The ternary equivalent of the AND operator is applied to each pair of trits
    in the input pair of Trytes.
    The AND operator (also known as the MIN operator) has the following truth table:

      AND | - | 0 | +
      -----------------
        - | - | - | -
        0 | - | 0 | 0
        + | - | 0 | +
    */
    std::array<int16_t, 9> this_array = this->ternary_array();
    std::array<int16_t, 9> other_array = other.ternary_array();

    for (size_t i = 0; i < 9; i++)
    {
        this_array[i] = std::min(this_array[i], other_array[i]);
    }
    return Tryte(this_array);
}
Tryte& Tryte::operator&=(Tryte const& other)
{
    *this = *this & other;
    return *this;
}
Tryte Tryte::operator|(Tryte const& other)
{
    /*
    The ternary equivalent of the OR operator is applied to each pair of trits
    in the input pair of Trytes.
    The OR operator (also known as the MAX operator) has the following truth table:

       OR | - | 0 | +
      -----------------
        - | - | 0 | +
        0 | 0 | 0 | +
        + | + | + | +
    */
    std::array<int16_t, 9> this_array = this->ternary_array();
    std::array<int16_t, 9> other_array = other.ternary_array();

    for (size_t i = 0; i < 9; i++)
    {
        this_array[i] = std::max(this_array[i], other_array[i]);
    }
    return Tryte(this_array);
}
Tryte& Tryte::operator|=(Tryte const& other)
{
    *this = *this | other;
    return *this;
}
Tryte Tryte::operator^(Tryte const& other)
{
    /*
    The ternary equivalent of the XOR operator is applied to each pair of trits
    in the input pair of Trytes. True if exactly one of the inputs is true, unknown
    if one or both is unknown, false otherwise.
    The XOR operator has the following truth table:

      XOR | - | 0 | +
      -----------------
        - | - | 0 | +
        0 | 0 | 0 | 0
        + | + | 0 | -
    */

    std::array<int16_t, 9> this_array = this->ternary_array();
    std::array<int16_t, 9> other_array = other.ternary_array();

    for (size_t i = 0; i < 9; i++)
    {
        if (this_array[i] == 0 or other_array[i] == 0)
        {
            this_array[i] = 0;
        }
        else if (this_array[i] == other_array[i])
        {
            this_array[i] = -1;
        }
        else
        {
            this_array[i] = 1;
        }
    }
    return Tryte(this_array);
}
Tryte& Tryte::operator^=(Tryte const& other)
{
    /*
    The ternary equivalent of the XOR operator is applied to each pair of trits
    in the input pair of Trytes.
    The XOR operator has the following truth table:

      XOR | - | 0 | +
      -----------------
        - | - | 0 | +
        0 | 0 | 0 | 0
        + | + | 0 | -
    */

    *this = *this ^ other;
    return *this;
}

Tryte Tryte::operator~()
{
    /*
    Ternary NOT operator. Truth table:

        A | - | 0 | +
        -----------------
       ~A | + | 0 | -
    */
   std::array<int16_t, 9> this_array = this->ternary_array();
   for (size_t i = 0; i < 9; i++)
   {
       this_array[i] = -this_array[i];
   }
   return Tryte(this_array);
}

Tryte Tryte::operator<<(uint16_t const& n)
{
    // tritshift left. New values at the right of the tryte are filled with zeroes.
    std::array<int16_t, 9> this_array = this->ternary_array();
    std::array<int16_t, 9> new_array;

    if (n >= 9)
    {
        // shifted completely to the left - or out of bounds
        return Tryte("000");
    }
    else
    {
        uint16_t k = 9 - n;
        for (size_t i = 0; i < k; i++)
        {
            new_array[i] = this_array[i + n];
        }
        // fill rest of array with zeroes
        for (size_t i = k; i < n; i++)
        {
            new_array[i] = 0;
        }
        // build a new Tryte with this new array
        return Tryte(new_array);
    }    
}
Tryte& Tryte::operator<<=(uint16_t const& n)
{
    *this = (*this << n);
    return *this;
}
Tryte Tryte::operator>>(uint16_t const& n)
{
    // tritshift right. New values at the left of the tryte are filled with zeroes.
    std::array<int16_t, 9> this_array = this->ternary_array();
    std::array<int16_t, 9> new_array;

    if (n >= 9)
    {
        // shifted completely to the right - or out of bounds
        return Tryte("000");
    }
    else
    {
        // fill new trits with zeroes
        for (size_t i = 0; i < n; i++)
        {
            new_array[i] = 0;
        }
        for (size_t i = n; i < 9; i++)
        {
            new_array[i] = this_array[i - n];
        }
        // build a new Tryte with this new array
        return Tryte(new_array);
    }    
}
Tryte& Tryte::operator>>=(uint16_t const& n)
{
    *this = (*this >> n);
    return *this;
}

std::ostream& operator<<(std::ostream& os, Tryte const& tryte)
{
    os << tryte.septavingt_string();
    return os;
}
std::istream& operator>>(std::istream& is, Tryte& tryte)
{
    std::string input;
    // Read input
    is >> input;
    if (is)
    {
        if (input.length() == 3)
        {
            Tryte temp(input.c_str());
            tryte = temp;
        }
        else if (input.length() == 9)
        {
            Tryte temp(input.c_str());
            tryte = temp;
        }
        else
        {
            // if read failed, set to zero.
            tryte.m_tryte = 0;
        }
    }
    return is;
}

Tryte Tryte::operator+(Tryte const& other)
{
    Tryte output("000");
    std::array<int16_t, 3> this_array = this->septavingt_array();
    std::array<int16_t, 3> other_array = other.septavingt_array();
    std::array<int16_t, 3> new_array = { 0, 0, 0 };
    int16_t carry = 0;

    // construct new array for the sum of Trytes
    for (size_t i = 0; i < 3; i++)
    {
        std::array<int16_t, 2> carry_and_sum =
            Tryte::carry_handler(this_array[2 - i] + other_array[2 - i] + carry);

        carry = carry_and_sum[0];
        new_array[2 - i] = carry_and_sum[1];
    }

    // construct output Tryte from new_array
    output = 729 * new_array[0] + 27 * new_array[1] + new_array[2];
    return output;
}
Tryte& Tryte::operator+=(Tryte const& other)
{
    (*this) = (*this) + other;
    return *this;
}
Tryte Tryte::operator-(Tryte const& other)
{
    Tryte output;
    output = (*this) + (-other);
    return output;
}
Tryte& Tryte::operator-=(Tryte const& other)
{
    (*this) = (*this) - other;
    return *this;
}
Tryte Tryte::operator-() const
{
    Tryte output;
    output.m_tryte = -this->m_tryte;
    return output;
}
Tryte Tryte::tritwise_add(Tryte const& t1, Tryte const& t2)
{
    std::array<int16_t, 9> t1_array = t1.ternary_array();
    std::array<int16_t, 9> t2_array = t2.ternary_array();
    std::array<int16_t, 9> new_array;
    for (size_t i = 0; i < 9; i++)
    {
        new_array[i] = t1_array[i] + t2_array[i];
        if (new_array[i] > 1)
        {
            new_array[i] -= 3;
        }
        if (new_array[i] < -1)
        {
            new_array[i] += 3;
        }
    }
    return Tryte(new_array);
}
Tryte Tryte::tritwise_mult(Tryte const& t1, Tryte const& t2)
{
    std::array<int16_t, 9> t1_array = t1.ternary_array();
    std::array<int16_t, 9> t2_array = t2.ternary_array();
    std::array<int16_t, 9> new_array;
    for (size_t i = 0; i < 9; i++)
    {
        new_array[i] = t1_array[i] * t2_array[i];
    }
    return Tryte(new_array);
}
std::array<Tryte, 2> Tryte::add_with_carry(Tryte const& t1, Tryte const& t2, Tryte const& carry)
{
    std::array<Tryte, 2> output = {0, 0};
    std::array<int16_t, 3> t1_array = t1.septavingt_array();
    std::array<int16_t, 3> t2_array = t2.septavingt_array();
    std::array<int16_t, 3> carry_array = carry.septavingt_array();
    std::array<int16_t, 4> new_array = {0, 0, 0, 0};
    int16_t new_carry = 0;

    // construct new_array for the sum of Trytes
    for (size_t i = 0; i < 3; i++)
    {
        std::array<int16_t, 2> carry_and_sum = 
        Tryte::carry_handler(t1_array[2 - i] + t2_array[2 - i] + carry_array[2 - i] + new_carry);

        new_carry = carry_and_sum[0];
        new_array[3 - i] = carry_and_sum[1];
    }
    new_array[0] = new_carry;

    // populate output Trytes using new_array 
    output[0] = new_array[0];
    output[1] = 729 * new_array[1] + 27 * new_array[2] + new_array[3];

    return output;
}
std::array<Tryte, 2> Tryte::mult(Tryte const& t1, Tryte const& t2)
{
    std::array<Tryte, 2> output = {0, 0};
    std::array<int16_t, 3> a = t1.septavingt_array();
    std::array<int16_t, 3> b = t2.septavingt_array();

    // Basically want to perform long multiplcation
    // in balanced septavingtesmal.
    // want to do
    //                         a0     a1     a2
    //                   *     b0     b1     b2
    // ----------------------------------------
    // carry  a0.b0  a1.b0  a2.b0      0      0 : s0
    //        carry  a0.b1  a1.b1  a2.b1      0 : s1
    //               carry  a0.b2  a1.b2  a2.b2 : s2
    // ----------------------------------------
    //    n0     n1     n2     n3     n4     n5

    // have to be careful to handle carries
    int16_t carry = 0;

    // create arrays to store intermediate results
    std::array<std::array<int16_t, 6>, 3> s;
    s[0] = {0, 0, 0, 0, 0, 0};
    s[1] = {0, 0, 0, 0, 0, 0};
    s[2] = {0, 0, 0, 0, 0, 0};
    std::array<int16_t, 2> temp = {0, 0};
    // and our final array
    std::array<int16_t, 6> n = {0, 0, 0, 0, 0, 0};

    // calculate s arrays
    for (size_t i = 0; i < 3; i++)
    {
        // calculate s[i]
        carry = 0;
        for (size_t j = 0; j < 3; j++)
        {
            temp = Tryte::carry_handler(a[2 - j] * b[i] + carry);
            carry = temp[0];
            int16_t result = temp[1];
            s[i][3 + i - j] += result;
        }
        s[i][i] = carry;
    }
    
    // now need to sum those s arrays to produce n

    carry = 0;
    for (size_t k = 0; k < 6; k++)
    {
        temp = Tryte::carry_handler(s[0][5 - k] + s[1][5 - k] + s[2][5 - k] + carry);
        carry = temp[0];
        n[5 - k] = temp[1];
    }

    // populate output Trytes using n
    output[0] = 729 * n[0] + 27 * n[1] + n[2];
    output[1] = 729 * n[3] + 27 * n[4] + n[5];

    return output; 
}