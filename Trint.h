#pragma once
#include <array>
#include <cstdint>
#include <ciso646>
#include <vector>
#include <string>
#include <algorithm>
#include <tuple>
#include <iostream> // for debugging
#include "Tryte.h"

template <size_t n>
class Trint
{
private:
	std::array<Tryte, n> _data;
	static const std::string septavingt_chars;

public:
	/*
	Constructors
	*/
	Trint()
	{
		for (size_t i = 0; i < n; i++)
		{
			_data[i] = 0;
		}
	}
	Trint(int64_t x)
	{
		/*
		first generate a septavingtesmal representation of x
		*/ 
		int64_t dividend = x > 0 ? x : -x;
		std::int16_t remainder = 0;
		std::vector<int16_t> values;

		while (dividend != 0)
		{
			remainder = dividend % 27;
			dividend = dividend / 27;
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
		if (x < 0)
		{
			for (auto& value : values)
			{
				value = -value;
			}
		}
		// pad with zeroes if x is too small for Trint
		if (values.size() < 3 * n)
		{
			for (size_t i = values.size(); i < 3 * n; i++)
			{
				values.push_back(0);
			}
		}

		/*
		use values to fill Trytes in Trint
		*/
		std::reverse(values.begin(), values.end());
		for (size_t tryte = 0; tryte < n; tryte++)
		{
			_data[tryte] = 729 * values[3 * tryte] + 27 * values[(3 * tryte) + 1] + values[(3 * tryte) + 2];
		}

	}
	Trint(Tryte const& tryte)
	{
		for (size_t i = 0; i < n; i++)
		{
			_data[i] = 0;
		}
		_data[n - 1] = tryte;
	}
	template <size_t m>
	Trint(Trint<m> const& trint)
	{
		if (m < n)
		{
			for (size_t i = 0; i < n - m; i++)
			{
				_data[i] = 0;
			}
			for (size_t i = n - m; i < n; i++)
			{
				_data[i] = trint[i - (n - m)];
			}
		}
		else if (m > n)
		{
			for (size_t i = 0; i < n; i++)
			{
				_data[i] = trint[(m - n) + i];
			}
		}
		else
		{
			for (size_t i = 0; i < n; i++)
			{
				_data[i] = trint[i];
			}
		}
	}
	Trint(std::array<Tryte, n>& tryte_array)
	{
		for (size_t i = 0; i < n; i++)
		{
			_data[i] = tryte_array[i];
		}
	}
	// for constructing from rvalue references
	Trint(std::array<Tryte, n>&& tryte_array)
	{
		for (size_t i = 0; i < n; i++)
		{
			_data[i] = tryte_array[i];
		}
	}

	/*
	subscript operators
	*/
	Tryte& operator[](size_t k)
	{
		return _data[k];
	}
	Tryte const& operator[](size_t k) const
	{
		return _data[k];
	}

	/*
	arithmetic operators
	*/
	Trint<n> operator+(Trint<n> const& other) const
	{
		Trint<n> output;
		Tryte carry;

		for (size_t i = 0; i < n; i++)
		{
			std::array<Tryte, 2> temp = Tryte::add_with_carry((*this)[n - i - 1], other[n - i - 1], carry);
			carry = temp[0];
			output[n - i - 1] = temp[1];
		}

		return output;
	}
	Trint<n>& operator+=(Trint<n> const& other)
	{
		*this = *this + other;
		return *this;
	}
	Trint<n> operator-() const
	{
		Trint<n> output = *this;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = -output[i];
		}
		return output;
	}
	Trint<n> operator-(Trint<n> const& other) const
	{
		Trint<n> output = *this;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = (*this)[i] - other[i];
		}
		return output;
	}
	Trint<n>& operator-=(Trint<n> const& other)
	{
		*this = *this - other;
		return *this;
	}
	Trint<n> operator*(Trint<n> const& other) const
	{
		// store intermediate results in array of Trints
		std::array<Trint<n>, n> products;

		// compute these products of single Trytes with the original Trint
		// and shift them as necessary
		for (size_t i = 0; i < n; i++)
		{
			products[i] = this->multiply_by_tryte(other[i]);
			products[i] = (products[i] << 3 * (n - i - 1));
		}

		// then just sum these shifted products
		Trint<n> output = 0;
		for (size_t i = 0; i < n; i++)
		{
			output += products[i];
		}
		return output;
	}
	Trint<n>& operator*=(Trint<n> const& other)
	{
		(*this) = (*this) * other;
		return *this;
	}

	/*
	increment/decrement operators
	*/
	Trint<n>& operator++()
	{
		*this += 1;
		return *this;
	}
	Trint<n> operator++(int)
	{
		Trint<n> temp(*this);
		operator++();
		return temp;
	}
	Trint<n>& operator--()
	{
		*this -= 1;
		return *this;
	}
	Trint<n> operator--(int)
	{
		Trint<n> temp(*this);
		operator--();
		return temp;
	}

	/*
	relational operators
	*/
	bool operator==(Trint<n> const& other) const
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] != other[i])
			{
				return false;
			}
		}
		return true;
	}
	bool operator!=(Trint<n> const& other) const
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] != other[i])
			{
				return true;
			}
		}
		return false;
	}
	bool operator<(Trint<n> const& other) const
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] < other[i])
			{
				return true;
			}
			else if ((*this)[i] > other[i])
			{
				return false;
			}
		}
		return false;
	}
	bool operator<=(Trint<n> const& other) const
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
	bool operator>(Trint<n> const& other) const
	{
		for (size_t i = 0; i < n; i++)
		{
			if ((*this)[i] > other[i])
			{
				return true;
			}
			else if ((*this)[i] < other[i])
			{
				return false;
			}
		}
		return false;
	}
	bool operator>=(Trint<n> const& other) const
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

	/*
	tritwise logical operators
	*/
	Trint<n> operator&(Trint<n> const& other) const
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ((*this)[i] & other[i]);
		}
		return output;
	}
	Trint<n>& operator&=(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			(*this)[i] = ((*this)[i] & other[i]);
		}
		return *this;
	}
	Trint<n> operator|(Trint<n> const& other) const
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ((*this)[i] | other[i]);
		}
		return output;
	}
	Trint<n>& operator|=(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			(*this)[i] = ((*this)[i] | other[i]);
		}
		return *this;
	}
	Trint<n> operator^(Trint<n> const& other) const
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ((*this)[i] ^ other[i]);
		}
		return output;
	}
	Trint<n>& operator^=(Trint<n> const& other)
	{
		for (size_t i = 0; i < n; i++)
		{
			(*this)[i] = ((*this)[i] ^ other[i]);
		}
		return *this;
	}
	Trint<n> operator~() const
	{
		Trint<n> output;
		for (size_t i = 0; i < n; i++)
		{
			output[i] = ~((*this)[i]);
		}
		return output;
	}

	/*
	tritwise shift operators
	*/
	Trint<n> operator<<(size_t const& k) const
	{
		// if we shift left too far, just get zero
		if (k >= 9 * n)
		{
			return 0;
		}

		// convert trint into a big array
		std::array<int16_t, 9 * n> big_tern_array = Trint<n>::ternary_array(*this);

		// now shift it
		for (size_t i = 0; i < 9 * n - k; i++)
		{
			big_tern_array[i] = big_tern_array[i + k];
		}
		for (size_t i = 9 * n - k; i < 9 * n; i++)
		{
			big_tern_array[i] = 0;
		}

		// and reconstruct our new Trint
		Trint<n> output = 0;
		for (size_t i = 0; i < n; i++)
		{
			std::array<int16_t, 9> tryte_tern_array;
			for (size_t j = 0; j < 9; j++)
			{
				tryte_tern_array[j] = big_tern_array[9 * i + j];
			}
			output[i] = Tryte::ternary_array_to_int(tryte_tern_array);
		}
		return output;

	}
	Trint<n>& operator<<=(size_t const& k)
	{
		*this = (*this << k);
		return *this;
	}
	Trint<n> operator>>(size_t const& k) const
	{
		// if we shift right too far, just get zero
		if (k >= 9 * n)
		{
			return 0;
		}

		// convert trint into a big array
		std::array<int16_t, 9 * n> big_tern_array = Trint<n>::ternary_array(*this);

		// now shift it
		for (size_t i = k; i < 9 * n; i++)
		{
			big_tern_array[i] = big_tern_array[i - k];
		}
		for (size_t i = 0; i < k; i++)
		{
			big_tern_array[i] = 0;
		}

		// and reconstruct our new Trint
		Trint<n> output = 0;
		for (size_t i = 0; i < n; i++)
		{
			std::array<int16_t, 9> tryte_tern_array;
			for (size_t j = 0; j < 9; j++)
			{
				tryte_tern_array[j] = big_tern_array[9 * i + j];
			}
			output[i] = Tryte::ternary_array_to_int(tryte_tern_array);
		}
		return output;
	}
	Trint<n>& operator>>=(size_t const& k)
	{
		*this = (*this >> k);
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, Trint<n> const& trint)
	{
		for (size_t i = 0; i < n; i++)
		{
			os << trint[i];
		}
		return os;
	}
	friend std::istream& operator>>(std::istream& is, Trint<n>& trint)
	{
		for (size_t i = 0; i < n; i++)
		{
			is >> trint[i];
		}
		return is;
	}

	Trint<n> multiply_by_tryte(Tryte const& t) const
	{
		Trint<n> output_trint = 0;
		Trint<n> carry_trint = 0;
		for (size_t k = 0; k < n; k++)
		{
			std::array<Tryte, 2> temp_mult = Tryte::mult((*this)[n - k - 1], t);
			if (k == n - 1)
			{
				output_trint[n - k - 1] = temp_mult[1];
			}
			else
			{
				carry_trint[n - k - 2] = temp_mult[0];
				output_trint[n - k - 1] = temp_mult[1];
			}

		}

		return output_trint + carry_trint;
	}

	static Trint<n> abs(Trint<n> const& x)
	{
		if (x > 0)
		{
			return x;
		}
		else if (x < 0)
		{
			return -x;
		}
		else
		{
			return 0;
		}
	}
	static int64_t sign(Trint<n> const& x)
	{
		if (x > 0)
		{
			return 1;
		}
		else if (x < 0)
		{
			return -1;
		}
		else
		{
			return 0;
		}		
	}

	static std::array<Trint<n>, 2> div(Trint<n> const& t1, Trint<n> const& t2)
	{
		// if t2 == 0, throw an error
		if (t2 == 0)
		{
			throw std::runtime_error("Attempted to divide by zero.");
		}

		// compute 'size' of t1 and t2 - number of digits
		int64_t size1 = 0;
		std::array<int16_t, 9 * n> tern_array1 = Trint<n>::ternary_array(t1);
		for (size_t i = 0; i < 9 * n; i++)
		{
			if (tern_array1[i] != 0 and size1 == 0)
			{
				size1++;
			}
			else if (size1 != 0)
			{
				size1++;
			}
		}
		int64_t size2 = 0;
		std::array<int16_t, 9 * n> tern_array2 = Trint<n>::ternary_array(t2);
		for (size_t i = 0; i < 9 * n; i++)
		{
			if (tern_array2[i] != 0 and size2 == 0)
			{
				size2++;
			}
			else if (size2 != 0)
			{
				size2++;
			}
		}

		// if size2 > size1, stop here
		if (size2 > size1)
		{
			std::array<Trint<n>, 2> output = {0, t1};
			return output;
		}

		int16_t shift = size1 - size2;
		Trint<n> dividend = t1;
		Trint<n> divisor = t2;
		Trint<n> quotient = 0;
		while (shift >= 0)
		{
			Trint<n> shift_up = dividend + (t2 << shift);
			Trint<n> shift_down = dividend + -(t2 << shift);

			std::array<Trint<n>, 3> test_array = 
			{Trint<n>::abs(shift_up), Trint<n>::abs(dividend), Trint<n>::abs(shift_down)};

			std::sort(test_array.begin(), test_array.end());
			Trint<n> min_element = test_array[0];
			
			if (min_element == Trint<n>::abs(shift_up))
			{
				// shift_up is smaller (disregarding signs)
				dividend = shift_up;
				quotient -= (Trint<n>(1) << shift);
				shift -= 1;
			}
			else if (min_element == Trint<n>::abs(shift_down))
			{
				// shift_down is smaller (disregarding signs)
				dividend = shift_down;
				quotient += (Trint<n>(1) << shift);
				shift -= 1;
			}
			else
			{
				// shifting up or down gets us further away from zero- do nothing
				shift -= 1;
			}
			std::cout << '\n';
		}
		Trint<n> remainder = dividend;

		// make remainder positive
		if (remainder < 0)
		{
			if (divisor < 0)
			{
				remainder -= divisor;
			}
			else
			{
				remainder += divisor;
			}
		}
		
		std::array<Trint<n>, 2> output = {quotient, remainder};
		return output;
	}

	static std::array<int16_t, 9 * n> ternary_array(Trint<n> const& x)
	{
		std::array<int16_t, 9 * n> output_tern_array;
		for (size_t i = 0; i < n; i++)
		{
			std::array<int16_t, 9> tryte_tern_array = Tryte::ternary_array(x[i]);
			for (size_t j = 0; j < 9; j++)
			{
				output_tern_array[9 * i + j] = tryte_tern_array[j];
			}
		}
		return output_tern_array;
	}
	static size_t length(Trint<n> const& t)
	{
		size_t output = 0;
		for (size_t i = 0; i < n; i++)
		{
			output += Tryte::length(t._data[i]);
		}
		return output;
	}
	static int64_t get_int(Trint<n> const& t)
	{
		int64_t output = 0;
		int64_t power_of_19683 = 1;
		for (size_t i = 0; i < n; i++)
		{
			output += Tryte::get_int(t[n - i - 1]) * power_of_19683;
			power_of_19683 *= 19683;
		}

		return output;
	}
};

template <size_t n>
std::string const Trint<n>::septavingt_chars = "MLKJIHGFEDCBA0abcdefghijklm";