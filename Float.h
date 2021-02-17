#pragma once
#include <string>
#include "Trint.h"

class TFloat
{
    private:
    Trint<1> _exponent;
    Trint<2> _mantissa;

    public:

    // constructors
    TFloat();
    TFloat(double d);
    TFloat(Trint<1> const& exponent, Trint<2> const& mantissa);
    TFloat(TFloat const& other);

    // relational operators
    bool operator==(TFloat const& other) const;
    bool operator!=(TFloat const& other) const;
    bool operator<(TFloat const& other) const;
    bool operator<=(TFloat const& other) const;
    bool operator>(TFloat const& other) const;
    bool operator>=(TFloat const& other) const;

    // stream operators
    friend std::ostream& operator<<(std::ostream& os, TFloat const& t);
    friend std::istream& operator>>(std::istream& is, TFloat const& t);

    // floating point arithmetic
    TFloat operator+(TFloat const& other) const;
    TFloat operator+=(TFloat const& other) const;
    TFloat operator-() const;
    TFloat operator-(TFloat const& other) const;
    TFloat operator-=(TFloat const& other) const;
    TFloat operator*(TFloat const& other) const;
    TFloat operator*=(TFloat const& other) const;
    TFloat operator/(TFloat const& other) const;
    TFloat operator/=(TFloat const& other) const;
    static TFloat abs(TFloat const& t);
    static bool isnan(TFloat const& t);

    // helpful functions
    static Trint<1> get_exponent(TFloat const& t);
    static Trint<2> get_mantissa(TFloat const& t);
    static double get_double(TFloat const& t);
};