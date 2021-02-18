#include "Float.h"
#include <array>
#include <cmath>
#include <algorithm>

// constructors
TFloat::TFloat()
{
    _exponent = 0;
    _mantissa = 0;
}
TFloat::TFloat(double d)
{
    // calculate exponent
    _exponent = 0;
    if (d > 0)
    {
        while (d > 1.5)
        {
            d /= 3;
            _exponent += 1;
        }
        while (d < 0.5)
        {
            d *= 3;
            _exponent -= 1;
        }
    }
    else if (d < 0)
    {
        while (d < -1.5)
        {
            d /= 3;
            _exponent += 1;
        }
        while (d > -0.5)
        {
            d *= 3;
            _exponent -= 1;
        }
    }

    // we now have -3/2 <= d <= -1/2 or 1/2 <= d <= 3/2
    // convert d into a normalised mantissa
    // fill mantissa array
    std::array<int16_t, 18> mantissa_array;
    mantissa_array.fill(0);
    double estimate = 0.0;
    double estimate_up = 0.0;
    double estimate_down = 0.0;
    for (size_t i = 0; i < 18; i++)
    {
        double power_of_3 = 1.0;
        // calculate current estimate
        for (size_t j = 0; j < i; j++)
        {
            estimate += mantissa_array[j] * (1.0 / power_of_3);
            power_of_3 *= 3.0;
        }

        // calculate upper and lower bounds
        estimate_up = estimate + (1.0 / power_of_3);
        estimate_down = estimate - (1.0 / power_of_3);

        // which one is closest to d?
        std::array<double, 3> estimate_array = 
        {std::abs(d - estimate_down), std::abs(d - estimate), std::abs(d - estimate_up)};
        double min_element = *std::min_element(estimate_array.begin(), estimate_array.end());

        if (min_element == estimate_array[0])
        {
            mantissa_array[i] = -1;
        }
        else if (min_element == estimate_array[1])
        {
            mantissa_array[i] = 0;
        }
        else
        {
            mantissa_array[i] = 1;
        }
    }

    // fill mantissa
    int64_t mantissa_int = 0;
    int64_t power_of_3 = 1;
    for (size_t i = 0; i < 18; i++)
    {
        mantissa_int = mantissa_array[i] * power_of_3;
        power_of_3 *= 3;
    }
    _mantissa = mantissa_int;
}
TFloat::TFloat(Trint<1> const& exponent, Trint<2> const& mantissa)
{
    _exponent = exponent;
    _mantissa = mantissa;
    // in case the input Trints are not formatted correctly, normalise
    this->normalise();
}
TFloat::TFloat(TFloat const& other)
{
    _exponent = other._exponent;
    _mantissa = other._mantissa;
}

// relational operators
bool TFloat::operator==(TFloat const& other) const
{
    if (_exponent == other._exponent and _mantissa == other._mantissa)
    {
        return true;
    }
    else
    {
        return false;
    }    
}
bool TFloat::operator!=(TFloat const& other) const
{
    if (_exponent != other._exponent or _mantissa != other._mantissa)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool TFloat::operator<(TFloat const& other) const
{
    // these should be zero if and only if this or other == 0
    int64_t this_sign = Trint<2>::sign(_mantissa);
    int64_t other_sign = Trint<2>::sign(other._mantissa);

    if (this_sign < other_sign)
    {
        return true;
    }
    else if (this_sign > other_sign)
    {
        return false;
    }
    else
    {
        // signs are equal
        if (this_sign == 1)
        {
            if (this->_exponent < other._exponent)
            {
                return true;
            }
            else if (this->_exponent > other._exponent)
            {
                return false;
            }
            else
            {
                // exponents are equal, compare mantissas
                if (this->_mantissa < other._mantissa)
                {
                    return true;
                }
                else if (this->_mantissa > other._mantissa)
                {
                    return false;
                }
                else
                {
                    // both floats are equal
                    return false;
                }
            }
        }
        else if (this_sign == -1)
        {
            if (this->_exponent < other._exponent)
            {
                return false;
            }
            else if (this->_exponent > other._exponent)
            {
                return true;
            }
            else
            {
                // exponents are equal, compare mantissas
                if (this->_mantissa < other._mantissa)
                {
                    return true;
                }
                else if (this->_mantissa > other._mantissa)
                {
                    return false;
                }
                else
                {
                    // both floats are equal
                    return false;
                }
            }
        }
        else
        {
            // both signs are zero, floats are equal
            return false;
        }   
    }
}
bool TFloat::operator<=(TFloat const& other) const
{
    if ((*this) < other or (*this) == other)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool TFloat::operator>(TFloat const& other) const
{
    // these should be zero if and only if this or other == 0
    int64_t this_sign = Trint<2>::sign(_mantissa);
    int64_t other_sign = Trint<2>::sign(other._mantissa);

    if (this_sign < other_sign)
    {
        return false;
    }
    else if (this_sign > other_sign)
    {
        return true;
    }
    else
    {
        // signs are equal
        if (this_sign == 1)
        {
            if (this->_exponent < other._exponent)
            {
                return false;
            }
            else if (this->_exponent > other._exponent)
            {
                return true;
            }
            else
            {
                // exponents are equal, compare mantissas
                if (this->_mantissa < other._mantissa)
                {
                    return false;
                }
                else if (this->_mantissa > other._mantissa)
                {
                    return true;
                }
                else
                {
                    // both floats are equal
                    return false;
                }
            }
        }
        else if (this_sign == -1)
        {
            if (this->_exponent < other._exponent)
            {
                return true;
            }
            else if (this->_exponent > other._exponent)
            {
                return false;
            }
            else
            {
                // exponents are equal, compare mantissas
                if (this->_mantissa < other._mantissa)
                {
                    return false;
                }
                else if (this->_mantissa > other._mantissa)
                {
                    return true;
                }
                else
                {
                    // both floats are equal
                    return false;
                }
            }
        }
        else
        {
            // both signs are zero, floats are equal
            return false;
        }   
    }
}
bool TFloat::operator>=(TFloat const& other) const
{
    if ((*this) > other or (*this) == other)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// stream operators
std::ostream& operator<<(std::ostream& os, TFloat const& t)
{
    // print exponent first
    os << t._exponent;

    // then print mantissa
    os << t._mantissa;

    return os;
}
std::istream& operator>>(std::istream& is, TFloat const& t)
{
    // fetch exponent first
    is >> t._exponent;

    // then fetch mantissa
    is >> t._mantissa;

    return is;
}

// floating point arithmetic
TFloat TFloat::operator+(TFloat const& other) const
{
    TFloat output = 0;
    int16_t shift = Tryte::get_int(this->_exponent[0] - other._exponent[0]);
    if (shift > 0)
    {
        if (shift > 18)
        {
            // other float is so much smaller than this one it's insignificant
            return *this;
        }
        else
        {
            // copy other into output and shift its mantissa and exponent to match this
            size_t shift_right = shift;
            output._mantissa = (other._mantissa >> shift_right);
            output._exponent = (other._exponent + shift_right);

            // now add this and other's mantissas (keeping track of carries!)
            std::array<Tryte, 2> temp;
            temp = Tryte::add_with_carry(this->_mantissa[1], output._mantissa[1], 0);
            output._mantissa[1] = temp[1];
            temp = Tryte::add_with_carry(this->_mantissa[0], output._mantissa[0], temp[0]);
            output._mantissa[0] = temp[1];

            // need to handle the carry
            int64_t carry_size = Tryte::size(temp[0]);
            if (carry_size > 0)
            {
                std::array<Tryte, 3> mantissa_array = {temp[0], output._mantissa[0], output._mantissa[1]};
                Trint<3> mantissa_with_carry(mantissa_array);

                // tritshift right the mantissa with carry until the left Tryte is zero (using its size)
                mantissa_with_carry >>= carry_size;
                // then add the shift to the exponent
                output._exponent[0] += carry_size;
            }
            
            // ensure output is normalised
            output.normalise();
            return output;
        }       
    }
    else if (shift < 0)
    {
        if (shift < -18)
        {
            // this float is so much smaller than other it's insignificant
            return other;
        }
        else
        {
            // copy this into output and shift its mantissa and exponent to match other
            size_t shift_right = -shift;
            output._mantissa = (this->_mantissa >> shift_right);
            output._exponent = (this->_exponent + shift_right);

            // now add this and other's mantissas (keeping track of carries!)
            std::array<Tryte, 2> temp;
            temp = Tryte::add_with_carry(other._mantissa[1], output._mantissa[1], 0);
            output._mantissa[1] = temp[1];
            temp = Tryte::add_with_carry(other._mantissa[0], output._mantissa[0], temp[0]);
            output._mantissa[0] = temp[1];

            // need to handle the carry
            int64_t carry_size = Tryte::size(temp[0]);
            if (carry_size > 0)
            {
                std::array<Tryte, 3> mantissa_array = {temp[0], output._mantissa[0], output._mantissa[1]};
                Trint<3> mantissa_with_carry(mantissa_array);

                // tritshift right the mantissa with carry until the left Tryte is zero (using its size)
                mantissa_with_carry >>= carry_size;
                // then add the shift to the exponent
                output._exponent[0] += carry_size;
            }

            // ensure output is normalised
            output.normalise();
            return output;
        }
    }
    else
    {
        // exponents match already, just need to add mantissas
        output._mantissa = this->_mantissa;
        output._exponent = this->_exponent;

        std::array<Tryte, 2> temp;
        temp = Tryte::add_with_carry(other._mantissa[1], output._mantissa[1], 0);
        output._mantissa[1] = temp[1];
        temp = Tryte::add_with_carry(other._mantissa[0], output._mantissa[0], temp[0]);
        output._mantissa[0] = temp[1];

        // need to handle the carry
        int64_t carry_size = Tryte::size(temp[0]);
        if (carry_size > 0)
        {
            std::array<Tryte, 3> mantissa_array = {temp[0], output._mantissa[0], output._mantissa[1]};
            Trint<3> mantissa_with_carry(mantissa_array);

            // tritshift right the mantissa with carry until the left Tryte is zero (using its size)
            mantissa_with_carry >>= carry_size;
            // then add the shift to the exponent
            output._exponent[0] += carry_size;
        }

        // normalise output
        output.normalise();

        return output;
    }
}
TFloat& TFloat::operator+=(TFloat const& other)
{
    *this = *this + other;
    return *this;
}
TFloat TFloat::operator-() const
{
    TFloat output;
    output._mantissa = -(this->_mantissa);
    output._exponent = this->_exponent;
    return output;
}
TFloat TFloat::operator-(TFloat const& other) const
{
    return *this + (-other);
}
TFloat& TFloat::operator-=(TFloat const& other)
{
    *this = *this - other;
    return *this;
}
TFloat TFloat::operator*(TFloat const& other) const
{
    // multiply mantissas, add exponents, and carry things as required
}

void TFloat::normalise()
{
    int64_t sign = Trint<2>::sign(_mantissa);
    if (sign > 0)
    {
        // +-------- = 3281
        while (_mantissa[0] < 3281)
        {
            _mantissa *= 3;
            _exponent -= 1;
        }
    }
    else if (sign < 0)
    {
        // -++++++++ = -3281
        while (_mantissa[0] > -3281)
        {
            _mantissa *= 3;
            _exponent -= 1;
        }
    }
    else
    {
        // float must be zero, set exponent to zero too
        _exponent = 0;
    }    
}
int64_t TFloat::sign(TFloat const& t)
{
    return Trint<2>::sign(t._mantissa);
}