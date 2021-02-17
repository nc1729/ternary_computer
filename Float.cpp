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

    if (_exponent < other._exponent)
    {
        // compare signs
    }
}